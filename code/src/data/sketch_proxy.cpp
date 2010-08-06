#include "onyx/cms/cms_utils.h"

#include "onyx/data/sketch_io.h"
#include "onyx/data/sketch_proxy.h"

namespace sketch
{

static const ZoomFactor ZOOM_ERROR = 0.001f;
static const int FAST_DRAWING_BUF_SIZE = 1;
static const int ERASE_UPDATE_INTERVAL = 300;
static const int DRIVER_DRAW_INTERVAL = 50;
static const int POINTS_NUM = 1;

// Transform the coordinate of sketch point by current orientation.
// The point in different rotation degrees would be transfered to the value
// as it works in 0 degree mode.
void transformCoordinate(const QRect & page_area,
                         const QPoint & stroke_point,
                         const RotateDegree orient,
                         QPoint & result)
{
    if (orient == ROTATE_0_DEGREE)
    {
        result = stroke_point;
        return;
    }

    int x = stroke_point.x();
    int y = stroke_point.y();
    switch (orient)
    {
    case ROTATE_90_DEGREE:
        result.setX(y);
        result.setY(page_area.width() - x);
        break;
    case ROTATE_180_DEGREE:
        result.setX(page_area.width() - x);
        result.setY(page_area.height() - y);
        break;
    case ROTATE_270_DEGREE:
        result.setX(page_area.height() - y);
        result.setY(x);
        break;
    default:
        break;
    }
}

// Transform the coordinate of sketch point by current orientation and zoom.
// The point in 0 degree widget would be transfered to the value in current
// rotation mode.
void transformCoordinate(const QRect & page_area,
                         const QPoint & stroke_point,
                         const RotateDegree orient,
                         QPoint & result,
                         const ZoomFactor zoom)
{
    int x = stroke_point.x();
    int y = stroke_point.y();
    if (zoom != 1.0f)
    {
        x = static_cast<int>(zoom * static_cast<ZoomFactor>(x));
        y = static_cast<int>(zoom * static_cast<ZoomFactor>(y));
    }

    switch (orient)
    {
    case ROTATE_0_DEGREE:
        result.setX(page_area.left() + x);
        result.setY(page_area.top() + y);
        break;
    case ROTATE_90_DEGREE:
        result.setX(page_area.left() + page_area.width() - y);
        result.setY(page_area.top() + x);
        break;
    case ROTATE_180_DEGREE:
        break;
    case ROTATE_270_DEGREE:
        result.setX(page_area.left() + y);
        result.setY(page_area.top() + page_area.height() - x);
        break;
    default:
        break;
    }
}

bool isPointValid(const QPoint & p)
{
    // TODO. use a more reasonable method
    return (p.x() >= 0 && p.y() >= 0);
}

void resetPoint(QPoint & p)
{
    p.setX(-1); p.setY(-1);
}

// Implementation of sketch agent
SketchProxy::SketchProxy()
    : attached_widget_(0)
    , need_update_once_(false)
    , mode_(MODE_SKETCHING)
    , status_(SKETCH_READY)
{
    resetLastPosition();

    // initialize all sketch handlers
    sketch_handlers_[MODE_SKETCHING][SKETCH_BEGIN] = &SketchProxy::beginSketch;
    sketch_handlers_[MODE_SKETCHING][SKETCH_MOVE] = &SketchProxy::moveSketch;
    sketch_handlers_[MODE_SKETCHING][SKETCH_END] = &SketchProxy::endSketch;
    sketch_handlers_[MODE_ERASING][SKETCH_BEGIN] = &SketchProxy::beginErase;
    sketch_handlers_[MODE_ERASING][SKETCH_MOVE] = &SketchProxy::moveErase;
    sketch_handlers_[MODE_ERASING][SKETCH_END] = &SketchProxy::endErase;

    // update erased area
    erase_update_timer_.setSingleShot( true );
    erase_update_timer_.setInterval( ERASE_UPDATE_INTERVAL );
    connect( &erase_update_timer_,
             SIGNAL( timeout() ),
             this,
             SLOT( onUpdateScreenTimeout() ) );

    // driver draw line
    driver_draw_timer_.setSingleShot( true );
    driver_draw_timer_.setInterval( DRIVER_DRAW_INTERVAL );
    connect( &driver_draw_timer_,
             SIGNAL( timeout() ),
             this,
             SLOT( onForceDriverDrawLines() ) );
}

SketchProxy::~SketchProxy()
{
    close();
}

bool SketchProxy::open(const QString & doc_path)
{
    // open the document with creation right
    SketchDocPtr doc = getDocument(doc_path);
    return doc != 0;
}

bool SketchProxy::close(const QString & doc_path)
{
    DocumentIter iter = docs_.find(doc_path);
    if (iter == docs_.end())
    {
        // if the document is not here, return false
        return false;
    }

    SketchIOPtr io = SketchIO::getIO(doc_path, false);
    if (io != 0)
    {
        io->close();
    }
    docs_.erase(iter);
    return true;
}

void SketchProxy::close()
{
    DocumentIter iter = docs_.begin();
    while (iter != docs_.end())
    {
        SketchIOPtr io = SketchIO::getIO( iter.value()->path(), false );
        if (io != 0)
        {
            io->close();
        }
        iter++;
    }
    docs_.clear();
}

bool SketchProxy::save(const QString & doc_path)
{
    // get the document
    SketchDocPtr doc = getDocument(doc_path);
    if (doc == 0)
    {
        // the document is invalid
        return false;
    }

    bool create_db = doc->hasDirtyPages();
    SketchIOPtr io = SketchIO::getIO(doc_path, create_db);
    if (io == 0)
    {
        // the database cannot be built
        return false;
    }

    PagesIter iter = doc->pages().begin();
    for (; iter != doc->pages().end(); iter++)
    {
        io->savePage(iter.value());
    }
    return true;
}

void SketchProxy::save()
{
    DocumentIter iter = docs_.begin();
    for (; iter != docs_.end(); iter++)
    {
        SketchDocPtr doc = *iter;
        save(doc->path());
    }
}

bool SketchProxy::exportDatabase(const QString & doc_path)
{
    // get the document
    SketchDocPtr doc = getDocument(doc_path);
    if (doc == 0)
    {
        // the document is invalid
        return false;
    }

    SketchIOPtr io = SketchIO::getIO(doc->path());
    if (io != 0)
    {
        return io->exportDatabase(doc->path());
    }
    return false;
}

bool SketchProxy::loadFromDatabase(const QString & db_name)
{
    QString sketch_postfix = cms::sketchPostfix();
    int index = db_name.indexOf(sketch_postfix);
    QString doc_name = db_name.left(index);
    return open(doc_name);
}

bool SketchProxy::loadPage(const QString & doc_path,
                           const PageKey & page_key,
                           const QString & background_image)
{
    // get the document
    SketchDocPtr doc = getDocument(doc_path);
    if (doc == 0)
    {
        // the document is invalid
        return false;
    }

    SketchPagePtr page = doc->getPage(page_key, true, background_image);
    // if the page is not recorded in DB or the data has been loaded,
    // do not access the DB
    if (page->dataLoaded())
    {
        return true;
    }

    if (page->isIDValid() && page->getStrokeCount() <= 0)
    {
        // get the io
        SketchIOPtr io = SketchIO::getIO(doc_path);
        if (io == 0)
        {
            return false;
        }
        return io->loadPageData(page);
    }

    // Do not try to load the page any more.
    page->setDataLoaded(true);
    return true;
}

bool SketchProxy::insertPage(const QString & doc_path,
                             const PageKey & page_key,
                             const QString & background_image)
{
    SketchDocPtr doc = getDocument(doc_path);
    if (doc == 0)
    {
        return false;
    }
    return doc->insertPage(page_key, background_image);
}

bool SketchProxy::removePage(const QString & doc_path, const PageKey & page_key)
{
    SketchDocPtr doc = getDocument(doc_path);
    if (doc == 0)
    {
        return false;
    }

    // get the io
    SketchIOPtr io = SketchIO::getIO(doc_path, false);
    return doc->removePage(page_key, io);
}

bool SketchProxy::isPageExisting(const QString & doc_path, const PageKey & page_key)
{
    SketchDocPtr doc = getDocument(doc_path);
    if (doc == 0)
    {
        return false;
    }

    SketchPagePtr page = doc->getPage(page_key, false, QString());
    if (page == 0)
    {
        return false;
    }
    return true;
}

bool SketchProxy::isEmptyPage(const QString & doc_path, const PageKey & page_key)
{
    SketchDocPtr doc = getDocument(doc_path);
    if (doc == 0)
    {
        return true;
    }

    SketchPagePtr page = doc->getPage(page_key, false, QString());
    if (page == 0)
    {
        return true;
    }
    return (page->getStrokeCount() == 0);
}

QString SketchProxy::getBackgroundImage(const QString & doc_path, const PageKey & page_key)
{
    SketchDocPtr doc = getDocument(doc_path);
    if (doc == 0)
    {
        return QString();
    }

    SketchPagePtr page = doc->getPage(page_key, false, QString());
    if (page == 0)
    {
        return QString();
    }
    return page->backgroundImage();
}

bool SketchProxy::updateBackgroundImage(const QString & doc_path,
                                        const PageKey & page_key,
                                        const QString & background_id)
{
    SketchDocPtr doc = getDocument(doc_path);
    if (doc == 0)
    {
        return false;
    }

    SketchPagePtr page = doc->getPage(page_key, true, QString());
    if (page == 0)
    {
        return false;
    }

    if (page->backgroundImage() != background_id)
    {
        page->setBackgroundImage(background_id);
        page->setBackgroundDirty(true);
    }
    return true;
}

int SketchProxy::getPageCount(const QString & doc_path)
{
    SketchDocPtr doc = getDocument(doc_path);
    if (doc == 0)
    {
        return 0;
    }
    return doc->getPageCount();
}

PageKey SketchProxy::getLastPage(const QString & doc_path)
{
    SketchDocPtr doc = getDocument(doc_path);
    if (doc == 0)
    {
        return 0;
    }
    return doc->getLastPage();
}

PageKey SketchProxy::getFirstPage(const QString & doc_path)
{
    SketchDocPtr doc = getDocument(doc_path);
    if (doc == 0)
    {
        return 0;
    }
    return doc->getFirstPage();
}

void SketchProxy::sketchPenDown(QMouseEvent *e)
{
    if (status_ == SKETCH_READY)
    {
        status_ = SKETCH_BEGIN;
    }
    SketchFunc func = sketch_handlers_[mode_][status_];
    (this->*func)(e);
}

void SketchProxy::sketchPenMove(QMouseEvent *e)
{
    if (status_ == SKETCH_BEGIN)
    {
        status_ = SKETCH_MOVE;
    }
    if (status_ == SKETCH_MOVE)
    {
        SketchFunc func = sketch_handlers_[mode_][status_];
        (this->*func)(e);
    }
}

void SketchProxy::sketchPenUp(QMouseEvent *e)
{
    status_ = SKETCH_END;
    SketchFunc func = sketch_handlers_[mode_][status_];
    (this->*func)(e);
    status_ = SKETCH_READY;
}

bool SketchProxy::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == attached_widget_ &&
        (event->type() == QEvent::MouseButtonPress ||
         event->type() == QEvent::MouseMove ||
         event->type() == QEvent::MouseButtonRelease))
    {
        QMouseEvent *mouse_event = down_cast<QMouseEvent *>(event);

        // check the drawing area
        if (!attached_widget_->rect().contains(mouse_event->pos()))
        {
            // the document or the page is invalid, end the last stroke and return
            if (stroke_ != 0 && last_page_ != 0)
            {
                endStroke(last_page_, last_pos_);
            }
            return true;
        }

        switch (event->type())
        {
        case QEvent::MouseButtonPress:
            sketchPenDown(mouse_event);
            break;
        case QEvent::MouseMove:
            sketchPenMove(mouse_event);
            break;
        case QEvent::MouseButtonRelease:
            sketchPenUp(mouse_event);
            break;
        default:
            break;
        }
        return true;
    }
    return QObject::eventFilter(obj, event);
}

void SketchProxy::attachWidget(QWidget * w)
{
    w->installEventFilter(this);
    attached_widget_ = w;
    setDrawingArea(w);
}

void SketchProxy::deattachWidget(QWidget * w)
{
    w->removeEventFilter(this);
    attached_widget_ = 0;
}

void SketchProxy::setDrawingArea(QWidget *w)
{
    gc_.setDrawingArea(w);
}

void SketchProxy::setMode(SketchMode mode)
{
    mode_ = mode;
}

void SketchProxy::setContentOrient(const RotateDegree r)
{
    gc_.setContentOrient(r);
}

void SketchProxy::setWidgetOrient(const RotateDegree r)
{
    gc_.setWidgetOrient(r);
}

void SketchProxy::setShape(const SketchShape s)
{
    sketch_ctx_.shape_ = s;
}

void SketchProxy::setColor(const SketchColor c)
{
    sketch_ctx_.color_ = c;
}

void SketchProxy::setZoom(const ZoomFactor z)
{
    sketch_ctx_.zoom_ = z;
}

void SketchProxy::setDrawLayer(const int l)
{
    sketch_ctx_.draw_layer_ = l;
}

void SketchProxy::paintPage(const QString & doc_path,
                            const PageKey & page_key,
                            QPainter & painter)
{
    SketchDocPtr doc = getDocument(doc_path);
    if (doc == 0)
    {
        return;
    }

    SketchPagePtr page = doc->getPage(page_key, false, QString());
    if (page == 0)
    {
        return;
    }
    page->paint(sketch_ctx_, gc_, painter);
}

bool SketchProxy::activateDocument(const QString & doc_path)
{
    SketchDocPtr doc = getDocument(doc_path);
    if (doc == 0)
    {
        return false;
    }

    DocumentIter idx = activated_docs_.find(doc_path);
    if (idx == activated_docs_.end())
    {
        activated_docs_[doc_path] = doc;
    }
    return true;
}

bool SketchProxy::activatePage(const QString & doc_path, const PageKey & page_key)
{
    if (activateDocument(doc_path))
    {
        SketchDocPtr doc = getDocument(doc_path);
        return doc->activatePage(page_key);
    }
    return false;
}

bool SketchProxy::deactivateDocument(const QString & doc_path)
{
    DocumentIter idx = activated_docs_.find(doc_path);
    if (idx != activated_docs_.end())
    {
        activated_docs_.erase(idx);
    }
    return true;
}

bool SketchProxy::deactivatePage(const QString & doc_path, const PageKey & page_key)
{
    SketchDocPtr doc = getDocument(doc_path);
    if (doc == 0)
    {
        return false;
    }
    return doc->deactivatePage(page_key);
}

void SketchProxy::deactivateAll()
{
    DocumentIter idx = activated_docs_.begin();
    while (idx != activated_docs_.end())
    {
        idx.value()->deactivateAll();
        idx++;
    }
    activated_docs_.clear();
}

bool SketchProxy::updatePageDisplayRegion(const QString & doc_path, const PageKey & page_key, const QRect & region)
{
    SketchDocPtr doc = getDocument(doc_path);
    if (doc == 0)
    {
        return false;
    }

    SketchPagePtr page = doc->getPage(page_key, true, QString());
    if (page == 0)
    {
        return false;
    }
    page->setDisplayArea(region);
    return true;
}

void SketchProxy::onUpdateScreenTimeout()
{
    emit requestUpdateScreen();
    need_update_once_ = false;
}

SketchDocPtr SketchProxy::getDocument(const QString & doc_path)
{
    DocumentIter idx = docs_.find(doc_path);
    if (idx != docs_.end())
    {
        return idx.value();
    }

    // create a new document
    SketchDocPtr doc(new SketchDocument());
    if (!doc->open(doc_path))
    {
        return SketchDocPtr();
    }

    // put new document instance into list
    docs_[doc_path] = doc;

    // get the list of pages id and initialize all of the page instances
    SketchIOPtr io = SketchIO::getIO(doc_path, false);
    if (io != 0)
    {
        doc->loadAllPages(io);
    }
    return doc;
}

bool SketchProxy::hitTest(QMouseEvent *e,
                          SketchDocPtr & hit_doc,
                          SketchPagePtr & hit_page,
                          SketchPosition & pos)
{
    DocumentIter idx = activated_docs_.begin();
    for (; idx != activated_docs_.end(); idx++)
    {
        if (idx.value()->hitTest(e, hit_page, pos))
        {
            hit_doc = idx.value();
            return true;
        }
    }
    return false;
}

bool SketchProxy::beginSketch(QMouseEvent *e)
{
    if (last_page_ != 0 && stroke_ != 0)
    {
        // if the last stroke has not ended yet, end it with last position
        endStroke(last_page_, last_pos_);
    }

    // hit test active document and page
    SketchPosition pos;
    SketchDocPtr doc;
    SketchPagePtr page;
    if (hitTest(e, doc, page, pos))
    {
        beginStroke(doc, page, pos);
        keepLastPosition(pos);
        keepLastPage(doc, page);
        return true;
    }
    return false;
}

bool SketchProxy::moveSketch(QMouseEvent *e)
{
    // hit test active document and page
    SketchPosition pos;
    SketchDocPtr doc(NULL);
    SketchPagePtr page(NULL);
    hitTest(e, doc, page, pos);
    if (!doc || !page)
    {
        // the document or the page is invalid, end the last stroke and return
        if (stroke_ != 0 && last_page_ != 0)
        {
            endStroke(last_page_, last_pos_);
            return true;
        }
        return false;
    }

    if (doc != last_doc_ || page != last_page_)
    {
        // if it is in a new document or a new page,
        // end the previous stroke if there is
        if (stroke_ != 0 && last_page_ != 0)
        {
            endStroke(last_page_, last_pos_);
        }
    }
    else if (stroke_ != 0)
    {
        // if it is in current document and in current page,
        // add this point
        addPoint(page, pos);
    }

    if (stroke_ == 0)
    {
        // begin a new stroke
        beginStroke(doc, page, pos);
    }

    // update the last position
    keepLastPosition(pos);
    keepLastPage(doc, page);
    return true;
}

bool SketchProxy::endSketch(QMouseEvent *e)
{
    // hit test active document and page
    SketchPosition pos;
    SketchDocPtr doc;
    SketchPagePtr page;
    hitTest(e, doc, page, pos);
    if (stroke_ != 0 && page == last_page_)
    {
        endStroke(page, pos);
        return true;
    }
    return false;
}

void SketchProxy::beginStroke(SketchDocPtr doc, SketchPagePtr page, const SketchPosition & pos)
{
    // update the sketch context
    doc->updateSketchContext(sketch_ctx_);

    // if the page is not dirty, set to be dirty
    if (!page->isDataDirty())
    {
        page->setDataDirty(true);
    }

    if (stroke_ == 0)
    {
        // create a new stroke
        SketchStrokePtr new_stroke(new SketchStroke(doc->sketchContext()));
        stroke_ = new_stroke;

        // add the new stroke to the page
        page->appendStroke(new_stroke);
    }

    emit strokeStarted();
    addPoint(page, pos);
}

void SketchProxy::addPoint(SketchPagePtr page, const SketchPosition & pos, bool is_last_point)
{
    assert(stroke_);

    // transform the sketch point
    QPoint point_without_rotate;
    transformCoordinate(page->displayArea(),
                        pos.sketch_pos,
                        gc_.contentOrient(),
                        point_without_rotate);

    // add point into stroke, record the offset
    SketchPoint point(point_without_rotate.x(), point_without_rotate.y());
    stroke_->addPoint(point);
    //qDebug("Add stroke point:(%d, %d), global point:(%d, %d)",
    //    point.x(), point.y(),
    //    pos.global_pos.x(), pos.global_pos.y());

    SketchContext ctx = sketch_ctx_;
    ctx.zoom_ = 1.0f;
    // draw the new point by driver
    if (isLastPositionValid())
    {
        driverDrawLine(last_pos_.global_pos,
                       pos.global_pos,
                       ctx,
                       is_last_point);
    }
    else
    {
        driverDrawLine(pos.global_pos,
                       pos.global_pos,
                       ctx,
                       is_last_point);
    }

    if (is_last_point)
    {
        resetLastPosition();
    }

    emit pointAdded(point);
}

void SketchProxy::endStroke(SketchPagePtr page, const SketchPosition & pos)
{
    // add the point into stroke
    addPoint(page, pos, true);
    emit strokeAdded(stroke_->points());

    // the current stroke is done, clear the reference
    stroke_ = SketchStrokePtr();
}

bool SketchProxy::beginErase(QMouseEvent *e)
{
    // hit test active document and page
    SketchPosition pos;
    SketchDocPtr doc;
    SketchPagePtr page;
    if (!hitTest(e, doc, page, pos))
    {
        return false;
    }

    // stop the timer to prevent update now
    erase_update_timer_.stop();

    // get the erase context
    EraseContext & ctx = doc->eraseContext();

    // reset the last point
    ctx.last_point_.reset();
    // TODO... refine these two lines
    ctx.size_ = static_cast<EraseSize>(sketch_ctx_.shape_);
    ctx.zoom_ = sketch_ctx_.zoom_;

    // update the record of last page
    keepLastPage(doc, page);

    Strokes erased_strokes;
    if (page->hitTest(pos.sketch_pos, ctx, erased_strokes))
    {
        // draw the erased strokes
        foreach (SketchStrokePtr ptr, erased_strokes)
        {
            ptr->setColor(SKETCH_COLOR_WHITE);
        }

        // fast draw the erased strokes
        driverDrawStrokes(page, erased_strokes, pos);

        // remove the erased strokes
        page->removeStrokes(erased_strokes);
        page->setDataDirty(true);
        need_update_once_ = true;
        return true;
    }
    return false;
}

bool SketchProxy::moveErase(QMouseEvent *e)
{
    // hit test active document and page
    SketchPosition pos;
    SketchDocPtr doc;
    SketchPagePtr page;
    if (!hitTest(e, doc, page, pos))
    {
        return false;
    }

    // get the erase context
    EraseContext & ctx = doc->eraseContext();
    if (doc != last_doc_ || page != last_page_)
    {
        ctx.last_point_.reset();
    }

    // update the record of last page
    keepLastPage(doc, page);
    Strokes erased_strokes;
    if (page->hitTest(pos.sketch_pos, ctx, erased_strokes))
    {
        // draw the erased strokes
        foreach (SketchStrokePtr ptr, erased_strokes)
        {
            ptr->setColor(SKETCH_COLOR_WHITE);
        }

        // fast draw the erased strokes
        driverDrawStrokes(page, erased_strokes, pos);

        // remove the erased strokes
        page->removeStrokes(erased_strokes);
        page->setDataDirty(true);
        need_update_once_ = true;
        return true;
    }
    return false;
}

bool SketchProxy::endErase(QMouseEvent *e)
{
    // hit test active document and page
    SketchPosition pos;
    SketchDocPtr doc;
    SketchPagePtr page;
    if (!hitTest(e, doc, page, pos))
    {
        return false;
    }

    bool ret = false;
    // get the erase context
    EraseContext & ctx = doc->eraseContext();
    Strokes erased_strokes;
    if (page->hitTest(pos.sketch_pos, ctx, erased_strokes))
    {
        // draw the erased strokes
        foreach (SketchStrokePtr ptr, erased_strokes)
        {
            ptr->setColor(SKETCH_COLOR_WHITE);
        }

        // fast draw the erased strokes
        driverDrawStrokes(page, erased_strokes, pos);

        // remove the erased strokes
        page->removeStrokes(erased_strokes);
        page->setDataDirty(true);
        need_update_once_ = true;
        ret = true;
    }

    if ( need_update_once_ )
    {
        // start the timer for updating the screen
        onyx::screen::instance().updateScreen(onyx::screen::ScreenProxy::DW, onyx::screen::ScreenCommand::WAIT_ALL);
        erase_update_timer_.start();
    }
    return ret;
}

void SketchProxy::driverDrawLines(StrokeLines &lines,
                                  const SketchContext & ctx,
                                  bool is_last_point)
{
    if (lines.empty() || (!is_last_point && lines.size() < POINTS_NUM))
    {
        driver_draw_timer_.start();
        return;
    }

    QVector<QPoint> points;
    qDebug("Driver draw lines:%d", lines.size());
    StrokeLineIter iter = lines.begin();
    points.push_back((*iter).p1);
    for (; iter != lines.end(); ++iter)
    {
        points.push_back((*iter).p2);
    }

    gc_.fastDrawLines(points, ctx);
    onyx::screen::instance().updateScreen(onyx::screen::ScreenProxy::DW, onyx::screen::ScreenCommand::WAIT_NONE);
    lines.clear();
}

void SketchProxy::onForceDriverDrawLines()
{
    qDebug("Force driver draw lines");
    SketchContext ctx = sketch_ctx_;
    ctx.zoom_ = 1.0f;
    driverDrawLines(stroke_lines_, ctx, true);
}

void SketchProxy::driverDrawLine(const QPoint & p1,
                                 const QPoint & p2,
                                 const SketchContext & ctx,
                                 bool is_last_point)
{
    QPoint real_p1(p1);
    QPoint real_p2(p2);
    QRect screen_area = gc_.getScreenRect();
    transformCoordinate(screen_area, p1, gc_.widgetOrient(), real_p1);
    transformCoordinate(screen_area, p2, gc_.widgetOrient(), real_p2);

    driver_draw_timer_.stop();
    stroke_lines_.push_back(StrokeLine(real_p1, real_p2));
    driverDrawLines(stroke_lines_, ctx, is_last_point);
}

// Fast draw several strokes at one time
// It can be used in erasing entire strokes.
void SketchProxy::driverDrawStrokes(SketchPagePtr page,
                                    const Strokes & strokes,
                                    const SketchPosition & pos)
{
    ZoomFactor cur_zoom = sketch_ctx_.zoom_;
    ZoomFactor ratio = 1.0f;
    SketchContext ctx = sketch_ctx_;

    // Calculate the offsets between global point and widget point
    QPoint pos_offset = pos.global_pos;
    pos_offset -= pos.widget_pos;
    //qDebug("BEGIN ERASING, Page Offset:%d %d\n\n\n", pos_offset.x(), pos_offset.y());

    // Draw each stroke
    foreach (SketchStrokePtr ptr, strokes)
    {
        Points & points = ptr->points();
        int count = points.size();
        if (count > 0)
        {
            ZoomFactor stroke_zoom = ptr->zoom();
            if (fabs(cur_zoom - stroke_zoom) > ZOOM_ERROR)
            {
                ratio = cur_zoom / stroke_zoom;
            }

            ctx.color_ = ptr->color();
            ctx.shape_ = ptr->shape();
            ctx.zoom_ = ratio;

            QPoint pos1, pos2;
            // get the point by content rotation
            int idx = 0;
            transformCoordinate(page->displayArea(),
                                points[idx],
                                gc_.contentOrient(),
                                pos1,
                                ratio);
            // get the global position of pos1
            pos1 += pos_offset;

            idx++;
            while (idx < (count - 1))
            {
                transformCoordinate(page->displayArea(),
                                    points[idx],
                                    gc_.contentOrient(),
                                    pos2,
                                    ratio);
                // get the global position of pos2
                pos2 += pos_offset;
                //qDebug("Erase stroke point:(%d, %d), global point:(%d, %d)",
                //    points[idx].x(), points[idx].y(),
                //    pos2.x(), pos2.y());

                driverDrawLine(pos1, pos2, ctx, false);
                idx++;
                pos1 = pos2;
            }

            // draw last point
            if (count <= 1)
            {
                driverDrawLine(pos1, pos1, ctx, true);
            }
            else
            {
                transformCoordinate(page->displayArea(),
                                    points[count - 1],
                                    gc_.contentOrient(),
                                    pos2,
                                    ratio);
                driverDrawLine(pos1, pos2, ctx, true);
            }
        }
    }
}

void SketchProxy::resetLastPosition()
{
    resetPoint(last_pos_.global_pos);
}

void SketchProxy::keepLastPosition(const SketchPosition & pos)
{
    last_pos_.widget_pos = pos.widget_pos;
    last_pos_.sketch_pos = pos.sketch_pos;
    last_pos_.global_pos = pos.global_pos;
}

void SketchProxy::keepLastPage(SketchDocPtr doc, SketchPagePtr page)
{
    last_doc_ = doc;
    last_page_ = page;
}

bool SketchProxy::isLastPositionValid()
{
    return isPointValid(last_pos_.global_pos);
}

}
