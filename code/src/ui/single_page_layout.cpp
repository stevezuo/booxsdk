#include "onyx/ui/single_page_layout.h"

using namespace ui;
namespace vbf
{

static const ZoomFactor REAL_ZOOM_MAX = ZOOM_MAX / ZOOM_ACTUAL;
static const ZoomFactor REAL_ZOOM_MIN = ZOOM_MIN / ZOOM_ACTUAL;

SinglePageLayout::SinglePageLayout(RotateDegree rotation,
                                   ZoomFactor zoom_setting)
    : PageLayout(rotation, zoom_setting)
    , display_area_()
    , cur_page_(0)
    , page_key_(0)
    , margin_()
{
}

SinglePageLayout::~SinglePageLayout()
{
}

void SinglePageLayout::update()
{
    // update the view area
    updateViewArea();

    if (page() != 0)
    {
        page()->updateDisplaySize(display_area_.crop.size(),
                                  PAGE_LAYOUT,
                                  pagesCache());
    }

    // update the page area
    updatePageArea();
}

bool SinglePageLayout::loadConfiguration(Configuration & conf)
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
        view_y = 0;
    }
    display_area_.view.moveTo(QPoint(view_x, view_y));
    return true;
}

bool SinglePageLayout::saveConfiguration(Configuration & conf)
{
    conf.options[CONFIG_PAGE_NUMBER]  = page_key_;
    conf.options[CONFIG_ZOOM_SETTING] = zoomSetting();
    conf.options[CONFIG_ROTATE_DEGREE] = pagesCache().rotation();
    conf.options[CONFIG_VIEW_X] = display_area_.view.left();
    conf.options[CONFIG_VIEW_Y] = display_area_.view.top();
    return true;
}

bool SinglePageLayout::setMargins(const MarginArea &margin)
{
    bool need_update = !(margin_ == margin);
    margin_ = margin;
    return need_update;
}

bool SinglePageLayout::setWidgetArea(const QRect &area)
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

void SinglePageLayout::updateViewArea()
{
    // shrink the widget area
    display_area_.crop.setLeft(display_area_.widget.left() + margin_.left);
    display_area_.crop.setRight(display_area_.widget.right() - margin_.right);
    display_area_.crop.setTop(display_area_.widget.top() + margin_.top);
    display_area_.crop.setBottom(display_area_.widget.bottom() - margin_.bottom);
}

void SinglePageLayout::setPage(int page_number, const QRect &actual_size)
{
    setPageToCache(page_number, actual_size);

    PagePtr page = pagesCache().getPage(page_number);
    page->updateDisplaySize(display_area_.crop.size(),
                            PAGE_LAYOUT,
                            pagesCache());

    updatePageArea();
}

void SinglePageLayout::setPageWithoutUpdate(int page_number,
                                            const QRect &actual_size)
{
    setPageToCache(page_number, actual_size);
    pagesCache().updatePage(page_number,
                            display_area_.crop.size(),
                            PAGE_LAYOUT);
}

PagePtr SinglePageLayout::page()
{
    if (!cur_page_ || cur_page_->key() != page_key_)
    {
        cur_page_ = pagesCache().getPage(page_key_);
    }
    return cur_page_;
}

void SinglePageLayout::setCurPage(const int cur_page_num)
{
    page_key_ = cur_page_num;
    cur_page_ = pagesCache().getPage(page_key_);
}

bool SinglePageLayout::moveViewportByCenter(const QPoint & center_point,
                                            const ZoomFactor delta_zoom)
{
    if (page() == 0)
    {
        return false;
    }

    // get new center point
    QPoint new_center_point = center_point * delta_zoom;
    if (pagesCache().zoomSetting() == ZOOM_HIDE_MARGIN)
    {
        QPoint pos_in_content;
        if (transformPageToContent(page()->key(), new_center_point, pos_in_content))
        {
            new_center_point = pos_in_content;
        }
    }

    // update the view area
    int content_width  = page()->displayArea().width();
    int content_height = page()->displayArea().height();
    int widget_width  = display_area_.widget.width();
    int widget_height = display_area_.widget.height();
    int view_width = 0;
    int view_height = 0;

    // set the coordination of page view, related to the widget
    if (content_width < widget_width)
    {
        view_width = content_width;
    }
    else
    {
        view_width = widget_width;
    }

    if (content_height < widget_height)
    {
        view_height = content_height;
    }
    else
    {
        view_height = widget_height;
    }

    // calculate the top left position of new view port by the new
    // center point.
    QPoint new_view_pos(new_center_point.x() - (view_width >> 1),
                        new_center_point.y() - (view_height >> 1));

    display_area_.view.moveTo(new_view_pos);
    return true;
}

void SinglePageLayout::clearPages()
{
    pagesCache().clear();
}

void SinglePageLayout::updatePageArea()
{
    PagePtr working_page = page();
    if (working_page == 0)
    {
        // emit need page signal
        emit needPageSignal(page_key_);
        return;
    }

    if (pagesCache().zoomSetting() == ZOOM_HIDE_MARGIN &&
        !working_page->contentArea().isValid())
    {
        emit needContentAreaSignal(working_page->key());
        return;
    }

    // update the size of content displaying area
    display_area_.content.setSize(working_page->displayArea().size());

    // update the view area
    int content_width  = display_area_.content.width();
    int content_height = display_area_.content.height();
    int widget_width  = display_area_.widget.width();
    int widget_height = display_area_.widget.height();

    // set the coordination of page view, related to the widget
    if (content_width < widget_width)
    {
        display_area_.view.setWidth(content_width);
        display_area_.content.moveLeft(display_area_.widget.left() +
                                       (widget_width - content_width) / 2);
    }
    else
    {
        display_area_.view.setWidth(widget_width);
        display_area_.content.moveLeft(display_area_.widget.left());
    }

    if (content_height < widget_height)
    {
        display_area_.view.setHeight(content_height);
        display_area_.content.moveTop(display_area_.widget.top() +
                                      (widget_height - content_height) / 2);
    }
    else
    {
        display_area_.view.setHeight(widget_height);
        display_area_.content.moveTop(display_area_.widget.top());
    }

    // update view port. View port must be inside the content area.
    // the coordination of view port is related to the crop area
    int view_left = display_area_.view.left();
    int view_top = display_area_.view.top();
    int view_height = display_area_.view.height();
    int view_width = display_area_.view.width();

    if (view_top < 0)
    {
        display_area_.view.moveTop(0);
    }

    if (view_top > (content_height - view_height))
    {
        display_area_.view.moveBottom(content_height);
    }

    if (view_left < 0)
    {
        display_area_.view.moveLeft(0);
    }

    if (view_left > content_width - view_width)
    {
        display_area_.view.moveRight(content_width);
    }

    // re-update the content position for displaying the margins
    if (content_height >= widget_height)
    {
        if (display_area_.view.top() == 0)
        {
            display_area_.content.moveTop(display_area_.crop.top());
        }

        if (display_area_.view.bottom() == content_height)
        {
            display_area_.content.moveBottom(display_area_.content.bottom() -
                                             margin_.bottom);
        }
    }

    if (content_width >= widget_width)
    {
        if (display_area_.view.left() == 0)
        {
            display_area_.content.moveLeft(display_area_.crop.left());
        }

        if (display_area_.view.right() == content_width)
        {
            display_area_.content.moveRight(display_area_.content.right() -
                                            margin_.right);
        }
    }

    // update the position of current page
    working_page->updateDisplayPosition(display_area_.view.topLeft());

    emit PageLayout::layoutDoneSignal();
}

void SinglePageLayout::transformWidgetToPage(const QPoint &src_pos,
                                             QPoint &dst_pos)
{
    // transfer to the position in page
    dst_pos.setX(src_pos.x() - display_area_.content.left() + display_area_.view.left());
    dst_pos.setY(src_pos.y() -  display_area_.content.top() + display_area_.view.top());
}

void SinglePageLayout::transformPageToWidget(const QPoint &src_pos,
                                             QPoint &dst_pos)
{
    // transfer to the position of crop(view_port)
    dst_pos.setX(src_pos.x() + display_area_.content.left() - display_area_.view.left());
    dst_pos.setY(src_pos.y() + display_area_.content.top() - display_area_.view.top());
}

bool SinglePageLayout::transformPageToContent(const int page_number,
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

bool SinglePageLayout::transformContentToPage(const int page_number,
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

bool SinglePageLayout::makeRectangleVisible(const int page_number,
                                            const QRect &rectangle)
{
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

    // the rectangle should be in the center of view port if possible
    int view_x = dst_rect.left() -
                 ((display_area_.view.width() - dst_rect.width()) >> 1);
    int view_y = dst_rect.top() -
                 ((display_area_.view.height() - dst_rect.height()) >> 1);

    bool ret = false;
    if (view_x != display_area_.view.left() ||
        view_y != display_area_.view.top())
    {
        display_area_.view.moveTo(view_x, view_y);
        ret = true;
    }

    // set current page number
    setCurPage(page_number);
    if (page() != 0)
    {
        page()->updateDisplaySize(display_area_.crop.size(),
                                  PAGE_LAYOUT,
                                  pagesCache());
    }

    updatePageArea();
    return ret;
}

void SinglePageLayout::pan(const int x_offset, const int y_offset)
{
    scroll(x_offset, y_offset);
}

void SinglePageLayout::scroll(const int x_offset, const int y_offset)
{
    static const int CONTENT_MAX = 0xffff;
    static const int OFFSET_ERROR = 5;
    static const int MOVE_ERROR = 2;
    if (qAbs(x_offset) < OFFSET_ERROR && qAbs(y_offset) < OFFSET_ERROR)
    {
        return;
    }

    int view_cursor_top = display_area_.view.top() + y_offset;
    int view_cursor_left = display_area_.view.left() + x_offset;
    int view_cursor_bottom = display_area_.view.bottom() + y_offset;
    int view_cursor_right = display_area_.view.right() + x_offset;
    int content_width = display_area_.content.width();
    int content_height = display_area_.content.height();

    if (y_offset < 0 && view_cursor_top < 0 &&
        view_cursor_top > (y_offset + MOVE_ERROR))
    {
        view_cursor_top = 0;
    }
    if (y_offset > 0 && view_cursor_bottom > content_height &&
        view_cursor_bottom < (content_height + y_offset - MOVE_ERROR))
    {
        view_cursor_top = content_height - display_area_.view.height();
        view_cursor_bottom = content_height;
        if (view_cursor_top < 0)
        {
            view_cursor_top = 0;
        }
    }

    if ((view_cursor_top < 0  &&
         qAbs(y_offset) > qAbs(x_offset)) ||
        (content_width <= display_area_.view.width() &&
         content_height <= display_area_.view.height() &&
         view_cursor_left < 0 &&
         qAbs(x_offset) > qAbs(y_offset)))
    {
        // move to previous page
        int prev_page_num = page_key_ - 1;
        if (prev_page_num < pagesCache().firstPageKey())
        {
            // has been to the first page, return directly
            return;
        }
        setCurPage(prev_page_num);

        // move to the bottom of previous page
        isLandscape() ? view_cursor_left = CONTENT_MAX :
                        view_cursor_top = CONTENT_MAX;
    }
    else if ((view_cursor_bottom > content_height &&
              qAbs(y_offset) > qAbs(x_offset)) ||
             (content_width <= display_area_.view.width() &&
              content_height <= display_area_.view.height() &&
              view_cursor_right > content_width &&
              qAbs(x_offset) > qAbs(y_offset)))
    {
        // get next page
        int next_page_num = page_key_ + 1;

        if (next_page_num > pagesCache().lastPageKey())
        {
            // has been to the last page, return directly
            // TODO. print the error message
            return;
        }

        setCurPage(next_page_num);

        // move to the top of next page
        isLandscape() ? view_cursor_left = 0 :
                        view_cursor_top = 0;
    }

    if (page() != 0)
    {
        page()->updateDisplaySize(display_area_.crop.size(),
                                  PAGE_LAYOUT,
                                  pagesCache());
    }

    display_area_.view.moveTo(view_cursor_left, view_cursor_top);
    updatePageArea();
}

void SinglePageLayout::jump(const int page_number,
                            const int x_offset,
                            const int y_offset)
{
    // search the page
    if (pagesCache().firstPageKey() < 0 ||
        pagesCache().lastPageKey() < 0 ||
        page_number < pagesCache().firstPageKey() ||
        page_number > pagesCache().lastPageKey())
    {
        page_key_ = page_number;
        return;
    }

    setCurPage(page_number);
    if (page() != 0)
    {
        page()->updateDisplaySize(display_area_.crop.size(),
                                  PAGE_LAYOUT,
                                  pagesCache());
    }

    updatePageArea();
}

bool SinglePageLayout::writeReadingHistory(QVariant & item)
{
    if (page() == 0)
    {
        qDebug("Cannot put invalid page into reading history");
        return false;
    }

    ReadingHistoryContext ctx;
    ctx.read_type = PAGE_LAYOUT;

    // get the center page
    ctx.page_number = page()->key();

    // get the view center in page coordination system
    QPoint center = display_area_.view.center();
    if (zoomSetting() == ZOOM_HIDE_MARGIN)
    {
        // if the previous mode is "Hide Margin",
        // content -> page
        QPoint center_in_page;
        if (transformContentToPage(page()->key(),
                                   center,
                                   center_in_page))
        {
            center = center_in_page;
        }
    }

    int page_width = page()->displayArea().width();
    int page_height = page()->displayArea().height();

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

void SinglePageLayout::restoreByReadingHistory(const QVariant & item)
{
    if (!item.canConvert<ReadingHistoryContext>())
    {
        qDebug("Invalid Reading History!");
        return;
    }

    ReadingHistoryContext ctx = item.value<ReadingHistoryContext>();

    // set the page_key to be page_number
    setCurPage(ctx.page_number);

    // restore the zoom setting
    pagesCache().setZoomSetting(ctx.zoom_setting);
    if (page() != 0)
    {
        page()->updateDisplaySize(display_area_.crop.size(),
                                  PAGE_LAYOUT,
                                  pagesCache());

        int page_width = page()->displayArea().width();
        int page_height = page()->displayArea().height();

        // move the view port
        // TODO. Take the rotation into consideration, otherwise the updated
        // view port is incorrect.
        QPoint center(static_cast<int>(ctx.view_center_in_page.x()) * page_width,
                      static_cast<int>(ctx.view_center_in_page.y()) * page_height);
        moveViewportByCenter(center, 1.0f);
    }
    updatePageArea();
}

bool SinglePageLayout::getContentPos(int page_number, QPoint &pos)
{
    if (page() != 0 && page_number == page()->key())
    {
        pos.setX(-(display_area_.view.left() - display_area_.content.left()));
        pos.setY(-(display_area_.view.top() - display_area_.content.top()));
        return true;
    }
    return false;
}

bool SinglePageLayout::getSpacingArea(int page_number, QRect &rect)
{
    return false;
}

bool SinglePageLayout::getViewPort(PagePtr page, QRect &result)
{
    result = display_area_.view;
    return true;
}

bool SinglePageLayout::getViewCropArea(QRect & crop_area)
{
    crop_area = display_area_.crop;
    return true;
}

void SinglePageLayout::rotate(const RotateDegree rotate_degree)
{
    pagesCache().rotate(rotate_degree);

    if (page() != 0)
    {
        page()->updateDisplaySize(display_area_.crop.size(),
                                  PAGE_LAYOUT,
                                  pagesCache());
    }

    updatePageArea();
}

void SinglePageLayout::zoomIn(const QRect &rect)
{
    assert(page());
    // the rect is in screen coordination
    // transfer the rect into content coordination
    QRect zoom_rect(rect);

    // transfer the rect into content coordination
    QPoint pos;
    transformWidgetToPage(rect.topLeft(), pos);
    zoom_rect.moveTo(pos);

    // get the intersected zoom rectangle
    zoom_rect = display_area_.view.intersected(zoom_rect);

    // calculate the new zoom factor
    ZoomFactor hor_zoom = static_cast<ZoomFactor>(display_area_.view.width()) /
                          static_cast<ZoomFactor>(zoom_rect.width());

    ZoomFactor ver_zoom = static_cast<ZoomFactor>(display_area_.view.height()) /
                          static_cast<ZoomFactor>(zoom_rect.height());

    // use the minimal one, it is now actually the increase value comparing
    // with current zoom
    ZoomFactor updated_zoom = std::min(hor_zoom, ver_zoom);
    ZoomFactor new_zoom = updated_zoom * page()->zoomValue();
    if (new_zoom > page()->maxZoomValue())
    {
        new_zoom = page()->maxZoomValue();
    }
    else if (new_zoom < REAL_ZOOM_MIN)
    {
        new_zoom = REAL_ZOOM_MIN;
    }
    updated_zoom = new_zoom / page()->zoomValue();

    // get the center point
    QPoint center = zoom_rect.center();
    if (zoomSetting() == ZOOM_HIDE_MARGIN)
    {
        // if it is in hide margin mode, transform the position of zoom rectangle
        // in page
        QPoint center_in_page;
        if (transformContentToPage(page()->key(),
                                   center,
                                   center_in_page))
        {
            center = center_in_page;
        }
    }

    pagesCache().setZoomSetting(new_zoom * ZOOM_ACTUAL);
    page()->updateDisplaySize(display_area_.crop.size(),
                              PAGE_LAYOUT,
                              pagesCache());

    // update the position of view port by the new zoom factor
    // The intersected selecting content would be expanded to the whole
    // view port. It means the position of view port(relating to content
    // coordination) should be updated, multipling by the new zoom facotr.
    moveViewportByCenter(center, updated_zoom);
    updatePageArea();
}

void SinglePageLayout::zoomToSpecialSetting(ZoomFactor setting)
{
    assert(page());

    // get the center point
    QPoint center = display_area_.view.center();
    if (zoomSetting() == ZOOM_HIDE_MARGIN)
    {
        // if the previous mode is "Hide Margin",
        // content -> page
        QPoint center_in_page;
        if (transformContentToPage(page()->key(),
                                   center,
                                   center_in_page))
        {
            center = center_in_page;
        }
    }

    pagesCache().setZoomSetting(setting);

    ZoomFactor prev_zoom = page()->zoomValue();
    page()->updateDisplaySize(display_area_.crop.size(),
                              PAGE_LAYOUT,
                              pagesCache());
    ZoomFactor real_zoom = page()->zoomValue() / prev_zoom;

    moveViewportByCenter(center, real_zoom);
    updatePageArea();
}

void SinglePageLayout::zoomToWidth()
{
    zoomToSpecialSetting(ZOOM_TO_WIDTH);
}

void SinglePageLayout::zoomToHeight()
{
    zoomToSpecialSetting(ZOOM_TO_HEIGHT);
}

void SinglePageLayout::zoomToVisible()
{
    zoomToSpecialSetting(ZOOM_HIDE_MARGIN);
}

void SinglePageLayout::zoomToBestFit()
{
    zoomToSpecialSetting(ZOOM_TO_PAGE);
}

void SinglePageLayout::setZoom(const ZoomFactor &value)
{
    if (value > ZOOM_MAX || value < ZOOM_MIN)
    {
        return;
    }

    // get the center point
    QPoint center = display_area_.view.center();
    if (zoomSetting() == ZOOM_HIDE_MARGIN)
    {
        // if it is in hide margin mode,
        // transform the viewport from content to page
        QPoint center_in_page;
        if (transformContentToPage(page()->key(),
                                   center,
                                   center_in_page))
        {
            center = center_in_page;
        }
    }

    ZoomFactor new_zoom = value / ZOOM_ACTUAL;
    if (new_zoom > page()->maxZoomValue())
    {
        new_zoom = page()->maxZoomValue();
    }
    else if (new_zoom < REAL_ZOOM_MIN)
    {
        new_zoom = REAL_ZOOM_MIN;
    }

    ZoomFactor real_zoom = new_zoom / page()->zoomValue();
    pagesCache().setZoomSetting(new_zoom * ZOOM_ACTUAL);
    page()->updateDisplaySize(display_area_.crop.size(),
                              PAGE_LAYOUT,
                              pagesCache());
    moveViewportByCenter(center, real_zoom);
    updatePageArea();
}

void SinglePageLayout::setContentArea(const int page_number,
                                      const QRect & rectangle)
{
    PagePtr page = pagesCache().getPage(page_number);
    assert(page != 0);
    page->setContentArea(rectangle);

    page->updateDisplaySize(display_area_.crop.size(),
                            PAGE_LAYOUT,
                            pagesCache());

    // update the visible pages
    updatePageArea();
}

const int SinglePageLayout::getCurrentPage()
{
    if (!page())
    {
        return -1;
    }

    return page()->key();
}

bool SinglePageLayout::hitTest(const QPoint &device_point,
                               QPoint & content_point,
                               int &page_number)
{
    if (!page())
    {
        return false;
    }

    transformWidgetToPage(device_point, content_point);
    if (page()->displayArea().contains(content_point))
    {
        if (zoomSetting() == ZOOM_HIDE_MARGIN)
        {
            // if it is in hide margin mode, transform the position of hit point
            // from content to page
            QPoint pos_in_page;
            if (transformContentToPage(page()->key(),
                                       content_point,
                                       pos_in_page))
            {
                content_point = pos_in_page;
            }
        }

        page_number = page()->key();
        return true;
    }

    return false;
}

void SinglePageLayout::getVisiblePages(VisiblePages &pages)
{
    pages.clear();
    if (page())
    {
        pages.push_back(page());
    }
}

}  // namespace vbf
