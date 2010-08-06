#include "onyx/ui/continuous_page_layout.h"

using namespace ui;
namespace vbf
{

static const int SPACE = 6;
static const ZoomFactor REAL_ZOOM_MAX = ZOOM_MAX / ZOOM_ACTUAL;
static const ZoomFactor REAL_ZOOM_MIN = ZOOM_MIN / ZOOM_ACTUAL;

ContinuousPageLayout::ContinuousPageLayout(RotateDegree rotation,
                                           ZoomFactor zoom_setting)
    : PageLayout(rotation, zoom_setting)
    , display_area_()
    , pages_()
    , margin_()
    , space_(SPACE)
    , page_key_(0)
{
}

ContinuousPageLayout::~ContinuousPageLayout()
{
}

bool ContinuousPageLayout::loadConfiguration(Configuration & conf)
{
    bool ok = false;
    page_key_ = conf.options[CONFIG_PAGE_NUMBER].toInt(&ok);
    if (!ok)
    {
        page_key_ = 0;
    }

    ui::ZoomFactor zoom_factor = conf.options[CONFIG_ZOOM_SETTING].toDouble(&ok);
    if (!ok)
    {
        zoom_factor = pagesCache().zoomSetting();
    }
    pagesCache().setZoomSetting(zoom_factor);

    RotateDegree rotate_degree = static_cast<RotateDegree>(
                                 conf.options[CONFIG_ROTATE_DEGREE].toInt(&ok));
    if (!ok)
    {
        rotate_degree = ROTATE_0_DEGREE;
    }
    pagesCache().rotate(rotate_degree);

    int view_x = conf.options[CONFIG_VIEW_X].toInt(&ok);
    if (!ok)
    {
        view_x = 0;
    }
    int view_y = conf.options[CONFIG_VIEW_Y].toInt(&ok);
    if (!ok)
    {
        view_y = topPosition();
    }
    display_area_.view.moveTo(QPoint(view_x, view_y));
    return true;
}

bool ContinuousPageLayout::saveConfiguration(Configuration & conf)
{
    if (visiblePages().empty())
    {
        conf.options[CONFIG_PAGE_NUMBER] = 0;
    }
    else
    {
        conf.options[CONFIG_PAGE_NUMBER] = visiblePages().front()->key();
    }

    conf.options[CONFIG_ZOOM_SETTING] = zoomSetting();
    conf.options[CONFIG_ROTATE_DEGREE] = pagesCache().rotation();
    conf.options[CONFIG_VIEW_X] = display_area_.view.left();
    conf.options[CONFIG_VIEW_Y] = display_area_.view.top();
    return true;
}


bool ContinuousPageLayout::setMargins(const MarginArea &margin)
{
    bool need_update = !(margin_ == margin);
    margin_ = margin;

    display_area_.view.moveTo(QPoint(0, topPosition()));
    return need_update;
}

bool ContinuousPageLayout::setWidgetArea(const QRect &area)
{
    if (!area.isValid())
    {
        return false;
    }

    if (display_area_.widget != area)
    {
        display_area_.widget = area;
    }
    return true;
}

void ContinuousPageLayout::clearPages()
{
    pagesCache().clear();

    visiblePages().clear();
    display_area_.content.setWidth(0);
    display_area_.content.setHeight(0);
}

void ContinuousPageLayout::resetVisibleArea()
{
    // set the position and size
    display_area_.crop.setLeft(display_area_.widget.left() + margin_.left);
    display_area_.crop.setRight(display_area_.widget.right() - margin_.right);
    display_area_.crop.setTop(display_area_.widget.top() + margin_.top);
    display_area_.crop.setBottom(display_area_.widget.bottom() - margin_.bottom);

    // update the position of content area
    // the position of content area should always be (0,0), actually the content
    // means the coordination of layout coordination system.
    display_area_.content.moveTo(QPoint(0,0));

    // update the size of view port
    display_area_.view.setWidth(display_area_.widget.width());
    display_area_.view.setHeight(display_area_.widget.height());

    // record the zoom of the first visible page if it exists
    PagePtr first_page = visiblePages().size() > 0 ? visiblePages().front() : 0;
    ZoomFactor prev_zoom = 0.0f;
    if (first_page != 0)
    {
        prev_zoom = first_page->zoomValue();
        // previous zoom should be bigger than 0
        assert(prev_zoom > 0.0001);
    }

    // TODO. Is it necessary to reset all pages every time?
    resetCachedPages();

    // move the view port only in special zoom setting
    if (first_page != 0 && pagesCache().zoomSetting() < 0)
    {
        // move the view port based on the delta of zoom value
        ZoomFactor new_zoom = first_page->zoomValue() / prev_zoom;

        // TODO. Move the viewport to appropriate position based on the
        // new center point
        display_area_.view.moveTo(
                static_cast<int>(
                static_cast<double>(display_area_.view.left()) * new_zoom),
                static_cast<int>(
                static_cast<double>(display_area_.view.top()) * new_zoom));
    }

    // update the visible pages
    updateVisibleArea();
}

void ContinuousPageLayout::setPage(int page_number, const QRect &actual_size)
{
    setPageToCache(page_number, actual_size);
    resetCachedPages();

    // TODO. to be removed
    PagePtr page = pagesCache().getPage(page_number);

    // initialize the visible pages list
    if (visiblePages().empty())
    {
        visiblePages().push_back(page);
    }

    // initialize the content area
    if (isLandscape())
    {
        if (display_area_.content.width() <= 0)
        {
            display_area_.content.setWidth(page->displayArea().width());
        }
    }
    else
    {
        if (display_area_.content.height() <= 0)
        {
            display_area_.content.setHeight(page->displayArea().height());
        }
    }

    updateVisibleArea();
}

void ContinuousPageLayout::setPageWithoutUpdate(int page_number,
                                                const QRect &actual_size)
{
    setPageToCache(page_number, actual_size);

    // DO NOT update all of the pages
    resetCachedPages(page_number);
}

void ContinuousPageLayout::pan(const int x_offset, const int y_offset)
{
    // the same as scroll
    scroll(x_offset, y_offset);
}

void ContinuousPageLayout::scroll(const int x_offset, const int y_offset)
{
#define OFFSET_ERROR 5
    if (qAbs(x_offset) < OFFSET_ERROR && qAbs(y_offset) < OFFSET_ERROR)
    {
        return;
    }

    display_area_.view.moveLeft(display_area_.view.left() + x_offset);
    display_area_.view.moveTop(display_area_.view.top() + y_offset);
    updateVisibleArea();
}

void ContinuousPageLayout::jump(const int page_number,
                                const int x_offset,
                                const int y_offset)
{
    // set the page_key to be page_number
    page_key_ = page_number;

    if (pagesCache().firstPageKey() < 0 ||
        pagesCache().lastPageKey() < 0 ||
        page_number < pagesCache().firstPageKey() ||
        page_number > pagesCache().lastPageKey())
    {
        return;
    }

    // jump to the page directly, display it at (0,0)
    visiblePages().clear();
    display_area_.content.setWidth(0);
    display_area_.content.setHeight(0);
    display_area_.view.moveTo(QPoint(0, 0));

    updateVisibleArea();
}

bool ContinuousPageLayout::writeReadingHistory(QVariant & item)
{
    ReadingHistoryContext ctx;
    ctx.read_type = CONTINUOUS_LAYOUT;

    // get the center page
    QPoint view_center = display_area_.view.center();
    PagePtr page = getCenterPage(view_center);
    if (page == 0)
    {
        return false;
    }
    ctx.page_number = page->key();

    // get the view center in page coordination system
    QPoint center = view_center;
    transformViewToPage(view_center, page, center);
    int page_width = page->displayArea().width();
    int page_height = page->displayArea().height();

    // TODO. Take the rotation into consideration, otherwise the updated view port
    // is incorrect.
    ctx.view_center_in_page.setX(static_cast<qreal>(center.x()) / page_width);
    ctx.view_center_in_page.setY(static_cast<qreal>(center.y()) / page_height);

    // set the zoom setting
    ctx.zoom_setting = pagesCache().zoomSetting();

    // set to the QVariant
    item.setValue(ctx);
    return true;
}

void ContinuousPageLayout::restoreByReadingHistory(const QVariant & item)
{
    if (!item.canConvert<ReadingHistoryContext>())
    {
        qDebug("Invalid Reading History!");
        return;
    }

    ReadingHistoryContext ctx = item.value<ReadingHistoryContext>();

    // set the page_key to be page_number
    page_key_ = ctx.page_number;

    // restore the zoom setting
    pagesCache().setZoomSetting(ctx.zoom_setting);

    PagePtr page = pagesCache().getPage(page_key_);
    if (page != 0)
    {
        resetCachedPages();

        // jump to the page directly
        visiblePages().clear();
        visiblePages().push_back(page);
        updateContentLength();

        int page_width = page->displayArea().width();
        int page_height = page->displayArea().height();

        // move the view port
        // TODO. Take the rotation into consideration, otherwise the updated
        // view port is incorrect.
        QPoint center(static_cast<int>(ctx.view_center_in_page.x()) * page_width,
                      static_cast<int>(ctx.view_center_in_page.y()) * page_height);

        moveViewportByCenter(page_key_, center, 1.0f);
    }
    updateVisibleArea();
}

bool ContinuousPageLayout::makeRectangleVisible(const int page_number,
                                                const QRect &rectangle)
{
    PagePtr page = 0;
    VisiblePagesIter idx = visiblePages().begin();
    for (; idx != visiblePages().end(); ++idx)
    {
        if ((*idx)->key() == page_number)
        {
            page = *idx;
            break;
        }
    }

    if (page == 0)
    {
        // cannot find the destination page, return
        return false;
    }

    QRect dst_rect = rectangle;

    // transform the position of rectangle in "Hide Margin" mode
    // page -> content
    if (zoomSetting() == ZOOM_HIDE_MARGIN)
    {
        QPoint pos_in_content;
        if (transformPageToContent(page_number,
                                   dst_rect.topLeft(),
                                   pos_in_content))
        {
            dst_rect.moveTo(pos_in_content);
        }
    }

    // page/content (hide margin) -> display area
    dst_rect.moveLeft(dst_rect.left() + page->displayArea().left());
    dst_rect.moveTop(dst_rect.top() + page->displayArea().top());

    // check whether the destination rectangle is visible
    int view_x = display_area_.view.left();
    int view_y = display_area_.view.top();

    if (dst_rect.width() < display_area_.view.width())
    {
        if (//is_landscape() &&
            (dst_rect.left() < display_area_.view.left() ||
             dst_rect.right() > display_area_.view.right()))
        {
            view_x = dst_rect.left() -
                     ((display_area_.view.width() - dst_rect.width()) >> 1);
        }
    }
    else
    {
        // Can use left margin?
        view_x = dst_rect.left() - margin_.left;
    }

    if (dst_rect.height() < display_area_.view.height())
    {
        if (//!is_landscape() &&
            (dst_rect.top() < display_area_.view.top() ||
             dst_rect.bottom() > display_area_.view.bottom()))
        {
            view_y = dst_rect.top() -
                ((display_area_.view.height() - dst_rect.height()) >> 1);
        }
    }
    else
    {
        view_y = dst_rect.top() - margin_.top;
    }

    if (view_x != display_area_.view.left() ||
        view_y != display_area_.view.top())
    {
        // move view port to the position of destination page
        display_area_.view.moveTo(view_x, view_y);
        updateVisibleArea();
        return true;
    }
    return false;
}

void ContinuousPageLayout::rotate(const RotateDegree rotate_degree)
{
    pagesCache().rotate(rotate_degree);

    // interchange the x and y of view port
    int view_top = display_area_.view.top();
    int view_left = display_area_.view.left();
    display_area_.view.moveTo(view_top, view_left);

    // reset all of the pages to for render setting update
    resetCachedPages();

    // update the length of content area
    int content_len = 0;
    VisiblePagesIter idx = visiblePages().begin();
    while (idx != visiblePages().end())
    {
        PagePtr page =*idx;
        content_len += (isLandscape() ? page->displayArea().width() :
                                        page->displayArea().height());
        content_len += space_;
        idx++;
    }
    if (content_len > 0)
    {
        content_len -= space_;
        // Only update the content area when visible pages are not empty
        isLandscape() ? display_area_.content.setWidth(content_len) :
                        display_area_.content.setHeight(content_len);
    }

    // update the visible pages
    updateVisibleArea();
}

void ContinuousPageLayout::zoomIn(const QRect &rect)
{
    if (visiblePages().size() == 0)
    {
        return;
    }

    // get the transformed position relating to the page
    // the rect is in screen coordination
    QRect zoom_rect = rect;

    // widget -> page / content(hide margin)
    QPoint pos;
    transformWidgetToPage(rect.topLeft(), pos);
    zoom_rect.moveTo(pos);

    // display area of all visible pages
    QRect visible_pages_area;
    PagePtr first_intersect_page = 0;
    VisiblePagesIter idx = visiblePages().begin();
    for (; idx != visiblePages().end(); ++idx)
    {
        if (zoom_rect.intersects((*idx)->displayArea()))
        {
            // valid visible page
            if (first_intersect_page == 0)
            {
                // get the first page
                first_intersect_page =*idx;
            }
        }

        // add the visible area of a page into the visible page area
        QRect visible_page_rect;
        getActualDisplayArea(*idx, visible_page_rect);
        visible_pages_area = visible_pages_area.united(visible_page_rect);
    }

    zoom_rect = zoom_rect.intersected(visible_pages_area);
    if (first_intersect_page == 0)
    {
        // no page is intersected with the zoom rectangle
        return;
    }

    // adjust the left/top position of zoom rectangle
    // If the content width/height is less than the width/height of view port
    // we should adjust the left/top position of zoom rectangle to keep the
    // correctness of view port's position
    QPoint center = zoom_rect.center();
    PagePtr center_page = getCenterPage(center);
    if (center_page == 0)
    {
        return;
    }

    if (!isLandscape())
    {
        zoom_rect.moveLeft(zoom_rect.left() - center_page->displayArea().left());
    }
    else
    {
        zoom_rect.moveTop(zoom_rect.top() - center_page->displayArea().top());
    }

    center = zoom_rect.center();
    if (!isLandscape())
    {
        center.setY(center.y() - center_page->displayArea().top());
    }
    else
    {
        center.setX(center.x() - center_page->displayArea().left());
    }


    if (zoomSetting() == ZOOM_HIDE_MARGIN)
    {
        // if it is in hide margin mode, transform the position of zoom rectangle
        // in page
        QPoint center_in_page;
        if (transformContentToPage(center_page->key(),
                                   center,
                                   center_in_page))
        {
            center = center_in_page;
        }
    }

    // get the new zoom factor
    // calculate the new zoom factor
    ZoomFactor hor_zoom = static_cast<ZoomFactor>(display_area_.view.width()) /
                          static_cast<ZoomFactor>(zoom_rect.width());
    ZoomFactor ver_zoom = static_cast<ZoomFactor>(display_area_.view.height()) /
                          static_cast<ZoomFactor>(zoom_rect.height());

    // use the minimal one, it is now actually the increase value comparing
    // with current zoom
    ZoomFactor updated_zoom = std::min(hor_zoom, ver_zoom);
    ZoomFactor new_zoom = center_page->zoomValue() * updated_zoom;
    if (new_zoom > center_page->maxZoomValue())
    {
        new_zoom = center_page->maxZoomValue();
    }
    else if (new_zoom < REAL_ZOOM_MIN)
    {
        new_zoom = REAL_ZOOM_MIN;
    }
    updated_zoom = new_zoom / center_page->zoomValue();

    // TODO. print error message and reset the zoom
    pagesCache().setZoomSetting(new_zoom * ZOOM_ACTUAL);

    // reset all of the pages
    resetCachedPages();

    // recalculate the length of content area;
    updateContentLength();

    visiblePages().clear();
    visiblePages().push_back(center_page);

    // move the view port
    // update the position of view port by the new zoom factor
    // The intersected selecting content would be expanded to the whole
    // view port. It means the position of view port(relating to content
    // coordination) should be updated, multipling by the new zoom facotor.
    moveViewportByCenter(center_page->key(), center, updated_zoom);

    // update the visible pages
    updateVisibleArea();
}

void ContinuousPageLayout::zoomToWidth()
{
    zoomToSpecialSetting(ZOOM_TO_WIDTH);
}

void ContinuousPageLayout::zoomToHeight()
{
    zoomToSpecialSetting(ZOOM_TO_HEIGHT);
}

void ContinuousPageLayout::zoomToBestFit()
{
    zoomToSpecialSetting(ZOOM_TO_PAGE);
}

void ContinuousPageLayout::zoomToSpecialSetting(ZoomFactor setting)
{
    // get first visible page
    if (visiblePages().empty())
    {
        return;
    }

    QPoint view_center = display_area_.view.center();
    PagePtr page = getCenterPage(view_center);
    QPoint center = view_center;
    transformViewToPage(view_center, page, center);

    visiblePages().clear();
    visiblePages().push_back(page);

    // get previous size of first visible page
    ZoomFactor prev_zoom = page->zoomValue();

    // update zoom setting to all pages
    pagesCache().setZoomSetting(setting);

    // reset all of the pages
    resetCachedPages();

    // recalculate the length of content area;
    updateContentLength();

    // get current size of first visible page
    ZoomFactor cur_zoom = page->zoomValue();

    // calculate the new zoom factor
    ZoomFactor real_zoom = cur_zoom / prev_zoom;

    // update the position of view port by the new zoom factor
    moveViewportByCenter(page->key(), center, real_zoom);

    // update the visible pages
    updateVisibleArea();
}

void ContinuousPageLayout::setZoom(const ZoomFactor &value)
{
    if (value > ZOOM_MAX || value < ZOOM_MIN)
    {
        return;
    }

    // get first visible page
    if (visiblePages().empty())
    {
        return;
    }

    QPoint view_center = display_area_.view.center();
    PagePtr page = getCenterPage(view_center);
    QPoint center = view_center;
    transformViewToPage(view_center, page, center);

    visiblePages().clear();
    visiblePages().push_back(page);

    ZoomFactor new_zoom = value / ZOOM_ACTUAL;
    if (new_zoom > page->maxZoomValue())
    {
        new_zoom = page->maxZoomValue();
    }
    else if (new_zoom < REAL_ZOOM_MIN)
    {
        new_zoom = REAL_ZOOM_MIN;
    }

    // calculate the real zoom
    ZoomFactor real_zoom = new_zoom / page->zoomValue();

    // TODO. print error message and reset the zoom
    pagesCache().setZoomSetting(new_zoom * ZOOM_ACTUAL);

    // reset all of the pages
    resetCachedPages();

    // recalculate the length of content area;
    updateContentLength();

    moveViewportByCenter(page->key(), center, real_zoom);

    // update the visible pages
    updateVisibleArea();
}

void ContinuousPageLayout::zoomToVisible()
{
    if (pagesCache().zoomSetting() == ZOOM_HIDE_MARGIN)
    {
        return;
    }

    // get the current page and clear all of the context
    QPoint view_center = display_area_.view.center();
    PagePtr center_page = getCenterPage(view_center);
    QPoint center = view_center;
    transformViewToPage(view_center, center_page, center);

    page_key_ = center_page->key();
    visiblePages().clear();
    display_area_.content.setWidth(0);
    display_area_.content.setHeight(0);

    ZoomFactor prev_zoom = center_page->zoomValue();

    // update zoom setting to all pages
    pagesCache().setZoomSetting(ZOOM_HIDE_MARGIN);

    // reset all of the cached pages
    resetCachedPages();

    // move the viewport to the apt position by center page and new center point.
    // at first get the center position
    hide_margin_ctx_.reset();
    if (!center_page->contentArea().isValid())
    {
        hide_margin_ctx_.center_page = center_page->key();
        hide_margin_ctx_.center_point = center;
        hide_margin_ctx_.prev_zoom = prev_zoom;
    }
    else
    {
        ZoomFactor delta_zoom = center_page->zoomValue() / prev_zoom;
        if (!moveViewportByCenter(center_page->key(), center, delta_zoom))
        {
            display_area_.view.moveTo(0, 0);
        }
    }

    // update the visible pages
    updateVisibleArea();
}

void ContinuousPageLayout::setContentArea(const int page_number,
                                          const QRect &rectangle)
{
    PagePtr page = pagesCache().getPage(page_number);
    assert(page != 0);
    page->setContentArea(rectangle);

    // update the zoom setting of page
    resetCachedPages(page_number);

    // update the length of content
    updateContentLength();

    // if it needs move the viewport, move it by center point
    if (hide_margin_ctx_.isValid() && page_number == hide_margin_ctx_.center_page)
    {
        ZoomFactor delta_zoom = page->zoomValue() / hide_margin_ctx_.prev_zoom;
        if (!moveViewportByCenter(page_number, hide_margin_ctx_.center_point, delta_zoom))
        {
            display_area_.view.moveTo(0, 0);
        }
        hide_margin_ctx_.reset();
    }

    // update the visible pages
    updateVisibleArea();
}

PagePtr ContinuousPageLayout::getCenterPage(const QPoint & center_point)
{
    VisiblePagesIter iter = visiblePages().begin();
    for (; iter != visiblePages().end(); ++iter)
    {
        if ((*iter)->displayArea().contains(center_point))
        {
            return (*iter);
        }
    }

    QRect center_area(display_area_.view.left(), center_point.y() - space_,
                      display_area_.view.width(), (space_ << 1));
    iter = visiblePages().begin();
    for (; iter != visiblePages().end(); ++iter)
    {
        if ((*iter)->displayArea().intersects(center_area))
        {
            return (*iter);
        }
    }
    return 0;
}

bool ContinuousPageLayout::moveViewportByCenter(const int center_page,
                                                const QPoint & center_point,
                                                const ZoomFactor delta_zoom)
{
    // get new center point
    QPoint new_center_point = center_point * delta_zoom;

    if (pagesCache().zoomSetting() == ZOOM_HIDE_MARGIN)
    {
        QPoint pos_in_content;
        if (transformPageToContent(center_page, new_center_point, pos_in_content))
        {
            new_center_point = pos_in_content;
        }
    }

    // calculate the top left position of new view port by the new
    // center point.
    QPoint new_view_pos(new_center_point.x() - (display_area_.view.width() >> 1),
                        new_center_point.y() - (display_area_.view.height() >> 1));

    display_area_.view.moveTo(new_view_pos);
    return true;
}

const int ContinuousPageLayout::getCurrentPage()
{
    // get the page with largest display area in current view port
    if (visiblePages().size() == 0)
    {
        return -1;
    }

    int cur_page = 0;
    VisiblePagesIter iter = visiblePages().begin();
    QRect largest_area;
    for (; iter != visiblePages().end(); ++iter)
    {
        QRect cur_area;
        getActualDisplayArea((*iter), cur_area);
        if (!isLandscape())
        {
            // compare the height
            if (cur_area.height() > largest_area.height())
            {
                cur_page = (*iter)->key();
                largest_area = cur_area;
            }
        }
        else
        {
            // compare the width
            if (cur_area.width() > largest_area.width())
            {
                cur_page = (*iter)->key();
                largest_area = cur_area;
            }
        }
    }
    return cur_page;
}

bool ContinuousPageLayout::hitTest(const QPoint &device_point,
                                   QPoint &content_point,
                                   int &page_number)
{
    if (visiblePages().size() == 0)
    {
        return false;
    }

    // widget -> page / content (hide margin)
    transformWidgetToPage(device_point, content_point);

    // estimate which page contains the point
    PagePtr page = 0;
    VisiblePagesIter idx = visiblePages().begin();
    for (; idx != visiblePages().end(); ++idx)
    {
        if ((*idx)->displayArea().contains(content_point))
        {
            page = *idx;
            break;
        }
    }

    if (page != 0)
    {
        if (zoomSetting() == ZOOM_HIDE_MARGIN)
        {
            // if it is in hide margin mode, transform the position of hit point
            // in page
            QPoint pos_in_page;
            if (transformContentToPage(page->key(),
                                       content_point,
                                       pos_in_page))
            {
                content_point = pos_in_page;
            }
        }

        content_point.setX(content_point.x() - page->displayArea().left());
        content_point.setY(content_point.y() - page->displayArea().top());
        page_number = page->key();
        return true;
    }
    return false;
}

void ContinuousPageLayout::update()
{
    resetVisibleArea();
}

void ContinuousPageLayout::getVisiblePages(VisiblePages &pages)
{
    pages.clear();
    VisiblePagesIter idx = visiblePages().begin();
    for(; idx != visiblePages().end(); ++idx)
    {
        pages.push_back(*idx);
    }
}

bool ContinuousPageLayout::getContentPos(int page_number, QPoint &pos)
{
    // the page should be in visible pages list
    PagePtr page = 0;
    VisiblePagesIter idx = visiblePages().begin();
    for(; idx != visiblePages().end(); ++idx)
    {
        if ((*idx)->key() == page_number)
        {
            page = *idx;
            break;
        }
    }

    if (page == 0)
    {
        return false;
    }

    // transfer the position of actual area into widget position
    transformPageToWidget(page->displayArea().topLeft(), pos);
    return true;
}

bool ContinuousPageLayout::getViewCropArea(QRect & crop_area)
{
    crop_area = display_area_.crop;
    return true;
}

bool ContinuousPageLayout::getSpacingArea(int page_number, QRect &rect)
{
    // the page should be in visible pages list
    PagePtr page = 0;
    VisiblePagesIter idx = visiblePages().begin();
    for(; idx != visiblePages().end(); ++idx)
    {
        if ((*idx)->key() == page_number)
        {
            page = *idx;
            break;
        }
    }

    if (page == 0)
    {
        return false;
    }

    QPoint spacing_pos = page->displayArea().bottomLeft();
    QRect spacing_rect(spacing_pos.x(),
                       spacing_pos.y(),
                       page->displayArea().width(),
                       space_);

    if (!display_area_.view.contains(spacing_rect))
    {
        return false;
    }

    // transfer the position of actual area into widget position
    transformPageToWidget(spacing_pos, spacing_pos);

    rect.setTopLeft(spacing_pos);
    rect.setWidth(spacing_rect.width());
    rect.setHeight(spacing_rect.height());
    return true;
}

bool ContinuousPageLayout::getViewPort(PagePtr page, QRect &result)
{
    getActualDisplayArea(page, result);
    if (!result.isValid())
    {
        return false;
    }

    // translate the position of intersected area into page postion
    result.moveLeft(result.left() - page->displayArea().left());
    result.moveTop(result.top() - page->displayArea().top());
    return true;
}

void ContinuousPageLayout::updateContentLength()
{
    PagePtr page = 0;
    int content_length = 0;
    VisiblePagesIter idx = visiblePages().begin();
    for (; idx != visiblePages().end(); ++idx)
    {
        page = *idx;
        content_length += (isLandscape() ? page->displayArea().width() :
                          page->displayArea().height());
        content_length += space_;
    }
    content_length -= space_;
    isLandscape() ? display_area_.content.setWidth(content_length) :
                    display_area_.content.setHeight(content_length);
}

void ContinuousPageLayout::resetCachedPages(int dst_page)
{
    if (pagesCache().size() <= 0)
    {
        // no page is cached, NOT a kind of error
        return;
    }

    if (dst_page < 0)
    {
        // if dst_page = -1, update all pages
        pagesCache().updateAllPages(display_area_.crop.size(),
                                    CONTINUOUS_LAYOUT);
    }
    else
    {
        // otherwise, update the destination page
        pagesCache().updatePage(dst_page,
                                display_area_.crop.size(),
                                CONTINUOUS_LAYOUT);
    }
    updateContentWidth();
}

void ContinuousPageLayout::updateContentWidth()
{
    // 1. adjust the vertical/horizontal position of each page by page mode
    int content_height = 0;
    int content_width = 0;
    PagePtr page = pagesCache().getFirstPage();
    if (!isLandscape())
    {
        while (page != 0)
        {
            if (page->displayArea().width() > content_width)
            {
                content_width = page->displayArea().width();
            }
            page = pagesCache().getNextPage(page->key());
        }
    }
    else
    {
        while (page != 0)
        {
            if (page->displayArea().height() > content_height)
            {
                content_height = page->displayArea().height();
            }
            page = pagesCache().getNextPage(page->key());
        }
    }

    // set the width/height of content
    isLandscape() ? display_area_.content.setHeight(content_height) :
                    display_area_.content.setWidth(content_width);

    // 2. Adjust the horizontal/vertical position of each page
    page = pagesCache().getFirstPage();
    if (!isLandscape())
    {
        // portrait mode
        while (page != 0)
        {
            int w = page->displayArea().width();
            // TODO. set x by alignment
            if (display_area_.view.width() > content_width)
            {
                page->displayArea().moveLeft((display_area_.view.width() - w) >> 1);
            }
            else if (content_width >= w)
            {
                page->displayArea().moveLeft((content_width - w) >> 1);
            }
            page = pagesCache().getNextPage(page->key());
        }
    }
    else
    {
        // landscape mode
        while (page != 0)
        {
            int h = page->displayArea().height();
            // TODO. set y by alignment
            if (display_area_.view.height() > content_height)
            {
                page->displayArea().moveTop((display_area_.view.height() - h) >> 1);
            }
            else if (content_height >= h)
            {
                page->displayArea().moveTop((content_height - h) >> 1);
            }
            page = pagesCache().getNextPage(page->key());
        }
    }
}

void ContinuousPageLayout::updateVisibleArea()
{
    if (visiblePages().size() <= 0)
    {
        PagePtr needed_page = pagesCache().getPage(page_key_);
        if (needed_page == 0)
        {
            emit needPageSignal(page_key_);
            return;
        }
        visiblePages().push_back(needed_page);
    }

    if (pagesCache().zoomSetting() == ZOOM_HIDE_MARGIN)
    {
        VisiblePagesIter iter = visiblePages().begin();
        while (iter != visiblePages().end())
        {
            if (!(*iter)->contentArea().isValid())
            {
                emit needContentAreaSignal((*iter)->key());
                return;
            }
            iter++;
        }
    }

    int view_start = isLandscape() ? display_area_.view.left() :
                     display_area_.view.top();
    int view_end = isLandscape() ? display_area_.view.right() :
                   display_area_.view.bottom();

    int content_start = (-1) * space_;
    int content_end = isLandscape() ? display_area_.content.right() :
                      display_area_.content.bottom();

    // 1. Get all visible pages, including the old ones
    PagePtr cur_page = 0;
    bool succeed = false;
    bool need_calc_prev = true;
    bool need_calc_next = true;
    while (!succeed)
    {
        if (view_start < content_start && need_calc_prev)
        {
            // expand the visible pages list
            cur_page = visiblePages().front();
            int pre_page_number = cur_page->key() - 1;
            if (pre_page_number < pagesCache().firstPageKey())
            {
                // reach to the first page
                // adjust the start position and end position of view port
                view_start = isLandscape() ? leftPosition() : topPosition();
                view_end = view_start + (isLandscape() ?
                           display_area_.view.width() :
                           display_area_.view.height());
                need_calc_prev = false;
            }
            else
            {
                cur_page = pagesCache().getPage(pre_page_number);
                if (cur_page == 0)
                {
                    emit needPageSignal(pre_page_number);
                    return;
                }

                if (pagesCache().zoomSetting() == ZOOM_HIDE_MARGIN &&
                    !cur_page->contentArea().isValid())
                {
                    emit needContentAreaSignal(cur_page->key());
                    return;
                }

                int inc_len = isLandscape() ? cur_page->displayArea().width() :
                              cur_page->displayArea().height() + space_;

                // reset the position of view port
                view_start += inc_len;
                content_end += inc_len;

                if (isLandscape())
                {
                    display_area_.view.moveLeft(view_start);
                    display_area_.content.setRight(content_end);
                    view_end = display_area_.view.right();
                }
                else
                {
                    display_area_.view.moveTop(view_start);
                    display_area_.content.setBottom(content_end);
                    view_end = display_area_.view.bottom();
                }

                visiblePages().push_front(cur_page);
            }
        }
        else if (view_end > content_end && need_calc_next)
        {
            // expand the visible pages list to add the new page
            cur_page = visiblePages().back();
            int next_page_number = cur_page->key() + 1;
            if (next_page_number > pagesCache().lastPageKey())
            {
                // reach to the last page
                // adjust the start position and end position of view port
                if (content_end <= 0)
                {
                    content_end = isLandscape() ? cur_page->displayArea().width() :
                                  cur_page->displayArea().height() + space_;
                }
                view_end = content_end;
                if (isLandscape())
                {
                    view_end += margin_.right;
                    display_area_.view.moveRight(view_end);
                    view_start = display_area_.view.left();
                }
                else
                {
                    view_end += margin_.bottom;
                    display_area_.view.moveBottom(view_end);
                    view_start = display_area_.view.top();
                }
                need_calc_next = false;
            }
            else
            {
                cur_page = pagesCache().getPage(next_page_number);
                if (cur_page == 0)
                {
                    emit needPageSignal(next_page_number);
                    return;
                }

                if (pagesCache().zoomSetting() == ZOOM_HIDE_MARGIN &&
                    !cur_page->contentArea().isValid())
                {
                    emit needContentAreaSignal(cur_page->key());
                    return;
                }

                int inc_len = isLandscape() ? cur_page->displayArea().width() :
                              cur_page->displayArea().height() + space_;

                // reset the size of content area
                content_end += inc_len;
                isLandscape() ? display_area_.content.setRight(content_end) :
                    display_area_.content.setBottom(content_end);

                visiblePages().push_back(cur_page);
            }
        }
        else
        {
            succeed = true;
        }
    }

    // 2. Shrink the visible pages
    int cur_pos = 0;
    int real_content_end = 0;
    VisiblePagesIter idx = visiblePages().begin();
    while (idx != visiblePages().end())
    {
        cur_page = *idx;
        int page_length = isLandscape() ? cur_page->displayArea().width() :
                          cur_page->displayArea().height();

        int start_pos = cur_pos;
        int end_pos = cur_pos + page_length;
        if (end_pos < view_start)
        {
            view_start -= (page_length + space_);
            view_end   -= (page_length + space_);
            idx = visiblePages().erase(idx);
        }
        else if (start_pos > view_end)
        {
            idx = visiblePages().erase(idx);
        }
        else
        {
            // set the top/left of current visible page
            isLandscape() ? cur_page->displayArea().moveLeft(real_content_end):
                            cur_page->displayArea().moveTop(real_content_end);

            // move to the top/left of next page
            real_content_end += (page_length + space_);
            cur_pos += (page_length + space_);
            idx++;
        }
    }
    real_content_end -= space_;

    if (!isLandscape())
    {
        display_area_.content.setBottom(real_content_end);
    }
    else
    {
        display_area_.content.setRight(real_content_end);
    }

    // adjust the viewport
    int origin_view_start = isLandscape() ? display_area_.view.left() :
                                            display_area_.view.top();
    int origin_view_end = isLandscape() ? display_area_.view.right() :
                                          display_area_.view.bottom();

    if (view_start != origin_view_start)
    {
        isLandscape() ? display_area_.view.moveLeft(view_start) :
                        display_area_.view.moveTop(view_start);
    }

    if (view_end != origin_view_end)
    {
        isLandscape() ? display_area_.view.moveRight(view_end) :
                        display_area_.view.moveBottom(view_end);
    }
    normalizeViewport();
}

void ContinuousPageLayout::normalizeViewport()
{
    if (!isLandscape())
    {
        // normalize the view port to keep the view port is in content area
        if (display_area_.view.top() < topPosition())
        {
            display_area_.view.moveTop(topPosition());
        }

        if (display_area_.view.bottom() > bottomPosition())
        {
            display_area_.view.moveBottom(bottomPosition());
        }

        if (display_area_.content.width() <= display_area_.view.width())
        {
            // normalize the view port to keep the content displaying in the
            // center
            display_area_.view.moveLeft(0);
        }
        else
        {
            if (display_area_.view.left() < leftPosition())
            {
                display_area_.view.moveLeft(leftPosition());
            }
            else if (display_area_.view.right() > rightPosition())
            {
                display_area_.view.moveRight(rightPosition());
            }
        }

        // if content area is smaller than the view port (only happens
        // when the number of page is small), make sure the content area
        // is centralized.
        if (display_area_.content.height() < display_area_.view.height())
        {
            int delta_len = (display_area_.view.height() -
                             display_area_.content.height()) >> 1;
            display_area_.view.moveTop(display_area_.content.top() -
                                       delta_len);
        }
    }
    else
    {
        // normalize the view port to keep the view port is in content area
        if (display_area_.view.left() < leftPosition())
        {
            display_area_.view.moveLeft(leftPosition());
        }

        if (display_area_.view.right() > rightPosition())
        {
            display_area_.view.moveRight(rightPosition());
        }

        // normalize the view port to keep the content displaying in the center
        if (display_area_.content.height() <= display_area_.view.height())
        {
            // normalize the view port to keep the content displaying in the
            // center
            display_area_.view.moveTop(0);
        }
        else
        {
            if (display_area_.view.top() < topPosition())
            {
                display_area_.view.moveTop(topPosition());
            }
            else if (display_area_.view.bottom() > bottomPosition())
            {
                display_area_.view.moveBottom(bottomPosition());
            }
        }

        // if content area is smaller than the view port (only happens
        // when the number of page is small), make sure the content area
        // is centralized.
        if (display_area_.content.width() < display_area_.view.width())
        {
            int delta_len = (display_area_.view.width() -
                             display_area_.content.width()) >> 1;
            display_area_.view.moveLeft(display_area_.content.left() - delta_len);
        }
    }
    // emit the layout done signal
    emit layoutDoneSignal();
}

void ContinuousPageLayout::transformWidgetToPage(const QPoint &src_pos,
                                                 QPoint &dst_pos)
{
    // transfer to the position in page
    dst_pos = src_pos + display_area_.view.topLeft();
}

void ContinuousPageLayout::transformPageToWidget(const QPoint &src_pos,
                                                 QPoint &dst_pos)
{
    // transfer to the position in widget
    dst_pos = src_pos - display_area_.view.topLeft();
}

bool ContinuousPageLayout::transformPageToContent(const int page_number,
                                                  const QPoint &src_pos,
                                                  QPoint &dst_pos)
{
    PagePtr page = pagesCache().getPage(page_number);
    if (page == 0)
    {
        return false;
    }

    QPoint content_pos;
    if (!getDisplayContentPosition(page->contentArea(),
                                   page->actualArea(),
                                   page->zoomValue(),
                                   rotateDegree(),
                                   content_pos))
    {
        return false;
    }

    dst_pos = src_pos - content_pos;
    return true;
}

bool ContinuousPageLayout::transformContentToPage(const int page_number,
                                                  const QPoint &src_pos,
                                                  QPoint &dst_pos)
{
    PagePtr page = pagesCache().getPage(page_number);
    if (page == 0)
    {
        return false;
    }

    QPoint content_pos;
    if (!getDisplayContentPosition(page->contentArea(),
                                   page->actualArea(),
                                   page->zoomValue(),
                                   rotateDegree(),
                                   content_pos))
    {
        return false;
    }

    dst_pos = src_pos + content_pos;
    return true;
}

void ContinuousPageLayout::transformViewToPage(const QPoint & src_pos,
                                               const PagePtr page,
                                               QPoint & dst_pos)
{
    assert(page != 0);
    dst_pos = src_pos - page->displayArea().topLeft();

    if (zoomSetting() == ZOOM_HIDE_MARGIN)
    {
        QPoint pos_in_page;
        if (transformContentToPage(page->key(), dst_pos, pos_in_page))
        {
            dst_pos = pos_in_page;
        }
    }
}

}  // namespace vbf
