#ifndef PAGE_LAYOUT_H_
#define PAGE_LAYOUT_H_

#include "onyx/base/base.h"
#include "onyx/ui/ui_global.h"
#include "onyx/data/configuration.h"

using namespace ui;

namespace vbf
{

static const int MAX_PAGE_SIZE = 12 * 1024 * 1024;

// Functions of "Hide-Margin"
bool getDisplayContentPosition(const QRect & origin_content_area,
                               const QRect & origin_page_area,
                               const ZoomFactor zoom_value,
                               const RotateDegree rotate_degree,
                               QPoint & content_pos);

bool getDisplayContentAreas(const QRect & origin_content_area,
                            const QRect & origin_page_area,
                            const ZoomFactor zoom_value,
                            const RotateDegree rotate_degree,
                            QRect & content_area,
                            QRect & out_bounding_area);

bool getZoomValueByContentArea(const QSize & region_size,
                               const QSize & content_size,
                               const PageLayoutType page_layout_type,
                               const RotateDegree rotate_degree,
                               ZoomFactor & zoom_value);

// Layout Page
class LayoutPages;
template <class K>
class Page
{
public:
    Page()
        : actual_area_()
        , content_area_()
        , display_area_()
        , zoom_value_(1.0f)
        , key_(0) {}

    Page(K k)
        : actual_area_()
        , content_area_()
        , display_area_()
        , zoom_value_(1.0f)
        , key_(k) {}

    Page(const Page & page)
        : actual_area_(page.actual_area_)
        , content_area_(page.content_area_)
        , display_area_(page.display_area_)
        , zoom_value_(page.zoom_value_)
        , key_(page.key_) {}

    ~Page() {}

    /// Set key of the page
    void setKey(K k) { key_ = k; }

    /// Get key of the page
    K key() const { return key_; }

    /// Set the actual size of the page
    void setActualArea(const QRect & r) { actual_area_ = r; }
    QRect & actualArea() { return actual_area_; }
    QRect & displayArea() { return display_area_; }
    const QPoint actualPosition() const { return actual_area_.topLeft(); }

    /// Set the content area of the page
    void setContentArea(const QRect & c) { content_area_ = c; }
    QRect & contentArea() { return content_area_; }

    /// Get the zoom value
    ZoomFactor zoomValue() const { return zoom_value_; }
    ZoomFactor maxZoomValue() const;

    /// Update the display position
    void updateDisplayPosition(const QPoint & pos) { display_area_.moveTo(pos); }

    /// Update the display size of the page
    /// NOTICE: the display size won't be calculated before calling this
    /// function
    void updateDisplaySize(const QSize & region,
                           const PageLayoutType page_layout_type,
                           LayoutPages & parent);

private:
    // actual size of current page
    // top & left : the rendering start position of the page.
    // (0,0) is the default setting
    QRect actual_area_;

    // content area of current page, in page coordination
    QRect content_area_;

    // display area, in device coordination
    // top & left : the displaying start position of the page,
    // in device coordination
    QRect display_area_;

    // current zoom value
    ZoomFactor zoom_value_;

    // key of the page
    K key_;
};

typedef Page<int> NumPage;
typedef NumPage* PagePtr;

struct MarginArea
{
    int left;
    int right;
    int top;
    int bottom;

    MarginArea()
        : left(1), right(1), top(1), bottom(1)
    {}

    MarginArea(int l, int r, int t, int b)
        : left(l), right(r), top(t), bottom(b)
    {}

    ~MarginArea() {}

    bool operator==(const MarginArea &right)
    {
        return (this->left == right.left
            && this->right == right.right
            && this->top == right.top
            && this->bottom == right.bottom);
    }
};

// A collection which caches all of the pages' layout information
class LayoutPages
{
public:
    LayoutPages();
    ~LayoutPages();

    /// add a new page into the cache
    bool setPage(PagePtr ptr);

    /// retrieve a cached page
    PagePtr getPage(int key);

    /// retrieve the next page by current index
    PagePtr getNextPage(int key);

    /// retrieve the first page
    PagePtr getFirstPage();

    /// get/set the key of first page
    void setFirstPageKey(const int k) { first_key_ = k; }
    int firstPageKey() { return first_key_; }

    /// get/set the key of last page
    void setLastPageKey(const int k) { last_key_ = k; }
    int lastPageKey() { return last_key_; }

    /// remove an existing page
    void removePage(int key);

    /// clear all cached page
    void clear();

    /// Set the zoom setting
    void setZoomSetting(const ZoomFactor z){ zoom_setting_ = z; }

    /// Rotate the page
    void rotate(const RotateDegree degree){ orientation_ = degree; }

    /// Get the view setting
    ZoomFactor zoomSetting() { return zoom_setting_; }
    RotateDegree rotation() { return orientation_; }

    /// Update the display area of all cached pages by current render setting
    void updateAllPages(const QSize &region,
                        const PageLayoutType page_layout_type);

    /// Update the display area of a cached page by current render setting
    void updatePage(const int page_number,
                    const QSize &region,
                    const PageLayoutType page_layout_type);

    /// get the number of cached pages
    int size() { return pages_.size(); }

private:
    typedef vector<PagePtr>     Pages;
    typedef Pages::iterator     PagesIter;

    static const unsigned int   vector_init_size;

private:
    // container
    Pages pages_;

    // current zoom, for every page
    ZoomFactor zoom_setting_;

    // rotation
    RotateDegree orientation_;

    // key of the first page
    int first_key_;

    // key of the last page
    int last_key_;

private:
    // reconstruct the vector
    void resetVector(int new_page_number);
};

typedef QList<PagePtr> VisiblePages;
typedef VisiblePages::iterator VisiblePagesIter;

// DisplayArea maintains all of the position data of visible areas
struct DisplayArea
{
    // the display area of widget, it includes margin
    QRect widget;

    // the display area of widget, without margin
    // coordination of crop is mapping to the widget
    QRect crop;

    // the display are of page
    // coordination of content is mapping to the widget
    QRect content;

    // the display area of view, it is view port
    // coordination of view is mapping to the content
    QRect view;

    DisplayArea()
        : widget(0, 0, 0, 0)
        , crop(0, 0, 0, 0)
        , content(0, 0, 0, 0)
        , view(0, 0, 0, 0) {}
    ~DisplayArea() {}
};

// Context of reading history
struct ReadingHistoryContext
{
    // number of the page
    int page_number;

    // center of the view in the page coordination system
    QPointF view_center_in_page;

    // zoom settomg
    ZoomFactor zoom_setting;

    // read mode
    PageLayoutType read_type;

    // TODO add support of rotation

    ReadingHistoryContext()
        : page_number(-1)
        , zoom_setting(INVALID_ZOOM)
        , read_type(INVALID_LAYOUT){}
    ~ReadingHistoryContext() {}
    bool isValid() { return page_number >= 0; }
};


// Base class of layout
class PageLayout : public QObject
{
    Q_OBJECT
public:
    PageLayout(RotateDegree rotation, ZoomFactor zoom_setting);
    ~PageLayout();

    /// load & save configurations
    virtual bool loadConfiguration(Configuration & conf) = 0;
    virtual bool saveConfiguration(Configuration & conf) = 0;

    /// set margins
    virtual bool setMargins(const MarginArea & margin) = 0;

    /// set the view area
    virtual bool setWidgetArea(const QRect & area) = 0;

    /// pan in the page
    virtual void pan(const int x_offset, const int y_offset) = 0;

    /// scroll int the page
    virtual void scroll(const int x_offset, const int y_offset) = 0;

    /// jump to another page
    virtual void jump(const int page_number,
                      const int x_offset = 0,
                      const int y_offset = 0) = 0;

    /// make sure the input rectangle(in specified page) is visible
    virtual bool makeRectangleVisible(const int page_number,
                                      const QRect &rectangle) = 0;

    /// rotate
    virtual void rotate(const RotateDegree rotate_degree) = 0;

    /// zoom in
    virtual void zoomIn(const QRect &rect) = 0;

    /// zoom to width
    virtual void zoomToWidth() = 0;

    /// zoom to height
    virtual void zoomToHeight() = 0;

    /// zoom to content
    virtual void zoomToVisible() = 0;
    virtual void setContentArea(const int page_number,
                                const QRect & rectangle) = 0;

    /// zoom to best fit
    virtual void zoomToBestFit() = 0;

    /// set constant zoom value
    virtual void setZoom(const ZoomFactor & value) = 0;

    /// get current page number
    virtual const int getCurrentPage() = 0;

    /// hittest
    virtual bool hitTest(const QPoint & device_point,
                         QPoint & content_point,
                         int & page_number) = 0;

    /// update by current setting
    virtual void update() = 0;

    /// get the visible pages
    virtual void getVisiblePages(VisiblePages & pages) = 0;

    /// get the position of the page int widget coordination system
    virtual bool getContentPos(int page_number, QPoint & pos) = 0;

    /// get the position and size of the spacing below a page
    /// this function is used for displaying content of an image
    virtual bool getSpacingArea(int page_number, QRect & rect) = 0;

    /// get the viewport
    virtual bool getViewPort(PagePtr page, QRect & result) = 0;

    /// get the crop area of view
    virtual bool getViewCropArea(QRect & crop_area) = 0;

    // add new page and update the display position
    virtual void setPage(int page_number, const QRect & actual_size) = 0;

    // add new page without update
    virtual void setPageWithoutUpdate(int page_number,
                                      const QRect & actual_size) = 0;

    // clear all cached pages
    virtual void clearPages() = 0;

    // write the reading history
    virtual bool writeReadingHistory(QVariant & item) = 0;

    // restore the viewport by given reading history
    virtual void restoreByReadingHistory(const QVariant & item) = 0;

    // get a page
    PagePtr getPage(const int key);

    // add a new page to cache
    void setPageToCache(int page_number, const QRect & actual_size);

    // get current zoom setting
    ZoomFactor zoomSetting() { return pagesCache().zoomSetting(); }

    // get current rotation degree
    RotateDegree rotateDegree() { return pagesCache().rotation(); }

    // set first page number
    void setFirstPageNumber(const int first);
    void setLastPageNumber(const int last);
    int getLastPageNumber();

Q_SIGNALS:
    /// layout done signal
    void layoutDoneSignal();

    /// need page signal
    void needPageSignal(const int page_number);

    /// need content area of page signal
    void needContentAreaSignal(const int page_number);

protected:
    LayoutPages & pagesCache() { return pages_cache_; }

private:
    // cached pages
    LayoutPages pages_cache_;
};

template <class K>
ZoomFactor Page<K>::maxZoomValue() const
{
    int actual_size  = actual_area_.width() * actual_area_.height();
    return static_cast<ZoomFactor>(MAX_PAGE_SIZE) / static_cast<ZoomFactor>(actual_size);
}

template <class K>
void Page<K>::updateDisplaySize(const QSize & region,
                                const PageLayoutType page_layout_type,
                                LayoutPages & parent)
{
    int actual_width  = actual_area_.width();
    int actual_height = actual_area_.height();
    if (parent.rotation() == ROTATE_90_DEGREE ||
        parent.rotation() == ROTATE_270_DEGREE)
    {
        actual_width = actual_area_.height();
        actual_height = actual_area_.width();
    }

    ZoomFactor zoom_h = 0.0, zoom_v = 0.0;
    QSize content_size;
    if (parent.zoomSetting() == ZOOM_TO_PAGE)
    {
        zoom_h = static_cast<ZoomFactor>(region.width()) /
                 static_cast<ZoomFactor>(actual_width);
        zoom_v = static_cast<ZoomFactor>(region.height()) /
                 static_cast<ZoomFactor>(actual_height);

        zoom_value_ = std::min(zoom_h, zoom_v);

    }
    else if (parent.zoomSetting() == ZOOM_TO_WIDTH)
    {
        zoom_h = static_cast<ZoomFactor>(region.width()) /
                 static_cast<ZoomFactor>(actual_width);

        zoom_value_ = zoom_h;
    }
    else if (parent.zoomSetting() == ZOOM_TO_HEIGHT)
    {
        zoom_v = static_cast<ZoomFactor>(region.height()) /
                 static_cast<ZoomFactor>(actual_height);

        zoom_value_ = zoom_v;
    }
    else if (parent.zoomSetting() == ZOOM_HIDE_MARGIN)
    {
        content_size = content_area_.size();
        if (parent.rotation() == ROTATE_90_DEGREE ||
            parent.rotation() == ROTATE_270_DEGREE)
        {
            content_size.transpose();
        }

        if (!getZoomValueByContentArea(region,
                                       content_size,
                                       page_layout_type,
                                       parent.rotation(),
                                       zoom_value_))
        {
            display_area_.setSize(QSize(0, 0));
            return;
        }
    }
    else
    {
        zoom_value_ = parent.zoomSetting() / ZOOM_ACTUAL;
    }

    static ZoomFactor max = ZOOM_MAX / ZOOM_ACTUAL;
    if (zoom_value_ > max)
    {
        zoom_value_ = max;
    }

    if (parent.zoomSetting() != ZOOM_HIDE_MARGIN)
    {
        display_area_.setWidth(static_cast<int>(zoom_value_ * actual_width));
        display_area_.setHeight(static_cast<int>(zoom_value_ * actual_height));
    }
    else
    {
        display_area_.setWidth(static_cast<int>(zoom_value_ * content_size.width()));
        display_area_.setHeight(static_cast<int>(zoom_value_ * content_size.height()));
    }
}

};  // namespace vbf

Q_DECLARE_METATYPE(vbf::ReadingHistoryContext);

#endif
