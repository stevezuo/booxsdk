#ifndef CONTINUOUS_PAGE_LAYOUT_H_
#define CONTINUOUS_PAGE_LAYOUT_H_

#include "onyx/ui/page_layout.h"

using namespace ui;

namespace vbf
{

class ContinuousPageLayout : public PageLayout
{
public:
    ContinuousPageLayout(RotateDegree rotation, ZoomFactor zoom_setting);
    ~ContinuousPageLayout();

    /// load from configuration
    virtual bool loadConfiguration(Configuration & conf);

    /// save layout data into configuration
    virtual bool saveConfiguration(Configuration & conf);

    /// set margins
    virtual bool setMargins(const MarginArea &margin);

    /// set the view area
    virtual bool setWidgetArea(const QRect &area);

    /// pan in the page
    virtual void pan(const int x_offset, const int y_offset);

    /// scroll int the page
    virtual void scroll(const int x_offset, const int y_offset);

    /// jump to another page
    virtual void jump(const int page_number,
                      const int x_offset = 0,
                      const int y_offset = 0);

    /// make sure the rectangle in a page is visible
    bool makeRectangleVisible(const int page_number,
                              const QRect &rectangle);

    /// rotate
    virtual void rotate(const RotateDegree rotate_degree);

    /// zoom in
    virtual void zoomIn(const QRect &rect);

    /// zoom to width
    virtual void zoomToWidth();

    /// zoom to height
    virtual void zoomToHeight();

    /// zoom to content
    virtual void zoomToVisible();
    virtual void setContentArea(const int page_number,
                                const QRect &rectangle);

    /// zoom to best fit
    virtual void zoomToBestFit();

    /// set fixed zoom value
    virtual void setZoom(const ZoomFactor &value);

    /// get current page number
    virtual const int getCurrentPage();

    /// hittest
    virtual bool hitTest(const QPoint &device_point,
                         QPoint &content_point,
                         int &page_number);

    /// update the layout by current setting
    virtual void update();

    /// get visible pages
    virtual void getVisiblePages(VisiblePages &pages);

    /// get the position of content
    virtual bool getContentPos(int page_number, QPoint &pos);

    /// get the spacing area of page
    virtual bool getSpacingArea(int page_number, QRect &rect);

    /// get the viewport
    virtual bool getViewPort(PagePtr page, QRect & result);

    /// get the crop area of view
    virtual bool getViewCropArea(QRect & crop_area);

    /// add a new page and update the display position
    virtual void setPage(int page_number, const QRect & actual_size);

    /// add new page without update
    virtual void setPageWithoutUpdate(int page_number,
                                      const QRect & actual_size);

    /// clear all cached pages and visible pages
    virtual void clearPages();

    /// write the reading history
    virtual bool writeReadingHistory(QVariant & item);

    /// restore the viewport by given reading history
    virtual void restoreByReadingHistory(const QVariant & item);

private:
    // update the crop area
    void resetVisibleArea();

    // reset the content area
    // if dst_page != -1, all of the cached pages would be updated
    // based on current zoom setting and crop size;
    // otherwise, only update the destine page.
    void resetCachedPages(int dst_page = -1);

    // update the length of content area based on current visible pages
    // NOTE: In landscape mode, the length means the width; in portrait mode,
    // the length means the height.
    void updateContentLength();

    // update the width of content area based on current visible pages
    // NOTE: In landscape mode, the width is actually the height
    void updateContentWidth();

    // nomorlize the position of view port
    void normalizeViewport();

    // update the number of first visible page
    void updateVisibleArea();

    // implementation of zoom to special setting
    void zoomToSpecialSetting(ZoomFactor setting);

    // get the center page
    PagePtr getCenterPage(const QPoint & center_point);

    // move view port by center point
    bool moveViewportByCenter(const int center_page,
                              const QPoint & center_point,
                              const ZoomFactor delta_zoom);

    // transform the point based in different trasform modes
    void transformWidgetToPage(const QPoint & src_pos,
                               QPoint & dst_pos);
    void transformPageToWidget(const QPoint & src_pos,
                               QPoint & dst_pos);
    bool transformPageToContent(const int page_number,
                                const QPoint & src_pos,
                                QPoint & dst_pos);
    bool transformContentToPage(const int page_number,
                                const QPoint & src_pos,
                                QPoint & dst_pos);
    void transformViewToPage(const QPoint & src_pos,
                             const PagePtr page,
                             QPoint & dst_pos);

    // get the actual display area of the page
    inline void getActualDisplayArea(const PagePtr page, QRect & area);

    // get visible pages
    inline VisiblePages & visiblePages() { return pages_; }

    // get the reading mode
    inline bool isLandscape();

    // get the top of visible area
    inline int topPosition() { return -margin_.top; }

    // get the bottom of visible area
    inline int bottomPosition() { return display_area_.content.bottom() +
                                  margin_.bottom; }

    // get the left position of visible area
    inline int leftPosition() { return -margin_.left; }

    // get the right position of visible area
    inline int rightPosition() { return display_area_.content.right() +
                                 margin_.right; }

private:
    struct HideMarginContext
    {
        int center_page;
        QPoint center_point;
        ZoomFactor prev_zoom;

        HideMarginContext() : center_page(-1), prev_zoom(ZOOM_ACTUAL) {}
        void reset() { center_page = -1; }
        bool isValid() { return center_page >= 0; }
    };

private:
    // display areas
    DisplayArea  display_area_;

    // visible pages
    VisiblePages pages_;

    // the margin area
    MarginArea   margin_;

    // the space between each page
    int          space_;

    // number of first visible page
    int          page_key_;

    // context of hide margin mode
    HideMarginContext hide_margin_ctx_;
};

inline void ContinuousPageLayout::getActualDisplayArea(const PagePtr page,
                                                       QRect &area)
{
    area = page->displayArea().intersected(display_area_.view);
}

inline bool ContinuousPageLayout::isLandscape()
{
    return pagesCache().rotation() == ROTATE_90_DEGREE ||
           pagesCache().rotation() == ROTATE_270_DEGREE;
}

};  // namespace vbf
#endif
