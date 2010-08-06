#ifndef SINGLE_PAGE_LAYOUT_H_
#define SINGLE_PAGE_LAYOUT_H_

#include "onyx/ui/page_layout.h"

using namespace ui;

namespace vbf
{

class SinglePageLayout : public PageLayout
{
public:
    SinglePageLayout(RotateDegree rotation, ZoomFactor zoom_setting);
    ~SinglePageLayout();

    /// load from configuration
    virtual bool loadConfiguration(Configuration & conf);

    /// save layout data into configuration
    virtual bool saveConfiguration(Configuration & conf);

    /// set margins
    virtual bool setMargins(const MarginArea & margin);

    /// set the view area
    virtual bool setWidgetArea(const QRect & area);

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
                              const QRect & rectangle);

    /// rotate
    virtual void rotate(const RotateDegree rotate_degree);

    /// zoom in
    virtual void zoomIn(const QRect & rect);

    /// zoom to width
    virtual void zoomToWidth();

    /// zoom to height
    virtual void zoomToHeight();

    /// zoom to content width
    virtual void zoomToVisible();
    virtual void setContentArea(const int page_number, const QRect & rectangle);

    /// zoom to best fit
    virtual void zoomToBestFit();

    /// set fixed zoom value
    virtual void setZoom(const ZoomFactor & value);

    /// get current page number
    virtual const int getCurrentPage();

    /// hittest
    virtual bool hitTest(const QPoint & device_point,
                         QPoint & content_point,
                         int & page_number);

    /// update the layout by current setting
    virtual void update();

    /// get visible pages
    virtual void getVisiblePages(VisiblePages & pages);

    /// get the position of content
    virtual bool getContentPos(int page_number, QPoint & pos);

    /// get the spacing area of a page
    virtual bool getSpacingArea(int page_number, QRect & rect);

    /// get the viewport
    virtual bool getViewPort(PagePtr page, QRect & result);

    /// get the crop area of view
    virtual bool getViewCropArea(QRect & crop_area);

    // add new page and update the display position
    virtual void setPage(int page_number, const QRect & actual_size);

    // add new page without update
    virtual void setPageWithoutUpdate(int page_number,
                                      const QRect & actual_size);

    /// clear all cached pages and visible pages
    virtual void clearPages();

    /// write the reading history
    virtual bool writeReadingHistory(QVariant & item);

    /// restore the viewport by given reading history
    virtual void restoreByReadingHistory(const QVariant & item);

private:
    // update the view area
    void updateViewArea();

    // update the page area
    void updatePageArea();

    // zoom to special setting
    void zoomToSpecialSetting(ZoomFactor setting);

    // get current page
    PagePtr page();

    // set current page
    void setCurPage(const int cur_page_num);

    // move view port by center point
    bool moveViewportByCenter(const QPoint & center_point,
                              const ZoomFactor delta_zoom);

    // transform the point based in different trasform modes
    void transformWidgetToPage(const QPoint &src_pos,
                               QPoint &dst_pos);
    void transformPageToWidget(const QPoint &src_pos,
                               QPoint &dst_pos);
    bool transformPageToContent(const int page_number,
                                const QPoint &src_pos,
                                QPoint &dst_pos);
    bool transformContentToPage(const int page_number,
                                const QPoint &src_pos,
                                QPoint &dst_pos);

    // get the reading mode
    bool isLandscape() { return pagesCache().rotation() == ROTATE_90_DEGREE ||
                                pagesCache().rotation() == ROTATE_270_DEGREE; }

private:
    // display areas
    DisplayArea display_area_;

    // current display page instance
    PagePtr     cur_page_;

    // number of current (needed) page
    int         page_key_;

    // the margin area
    MarginArea margin_;
};

};  // namespace vbf

#endif
