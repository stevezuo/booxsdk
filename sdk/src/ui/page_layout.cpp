#include "onyx/ui/page_layout.h"

using namespace ui;
namespace vbf
{

const unsigned int LayoutPages::vector_init_size = 600;

LayoutPages::LayoutPages()
    : pages_(vector_init_size, 0)
    , zoom_setting_(ZOOM_ACTUAL)
    , orientation_(ROTATE_0_DEGREE)
    , first_key_(-1)
    , last_key_(-1)
{
}

LayoutPages::~LayoutPages()
{
    clear();
}

bool LayoutPages::setPage(PagePtr ptr)
{
    int key = ptr->key();
    if (key < static_cast<int>(pages_.size()))
    {
        if (pages_[key] != 0)
        {
            printf("The page has been cached\n");
            delete pages_[key];
        }
    }
    else
    {
        resetVector(key);
    }
    pages_[key] = ptr;
    return true;
}

PagePtr LayoutPages::getPage(int key)
{
    if (key < 0 || key >= static_cast<int>(pages_.size()))
    {
        return 0;
    }

    return pages_[key];
}

PagePtr LayoutPages::getNextPage(int key)
{
    if (key >= static_cast<int>(pages_.size()))
    {
        return 0;
    }

    PagePtr page = 0;
    int idx = key;
    while (page == 0 && idx < static_cast<int>(pages_.size() -1))
    {
        page = pages_[++idx];
    }
    return page;
}

PagePtr LayoutPages::getFirstPage()
{
    PagePtr page = 0;
    int idx = 0;
    while (page == 0 && idx < static_cast<int>(pages_.size() - 1))
    {
        page = pages_[idx++];
    }
    return page;
}

void LayoutPages::removePage(int key)
{
    if (key < 0 || key >= static_cast<int>(pages_.size()))
    {
        return;
    }

    if (key < static_cast<int>(pages_.size()) && pages_[key] != 0)
    {
        delete pages_[key];
        pages_[key] = 0;
    }
}

void LayoutPages::clear()
{
    PagesIter iter = pages_.begin();
    for(; iter != pages_.end(); ++iter)
    {
        delete (*iter);
        (*iter) = 0;
    }

    // shrink the vector
    pages_.resize(vector_init_size, 0);
}

void LayoutPages::updateAllPages(const QSize &region,
                                 const PageLayoutType page_layout_type)
{
    PagesIter iter = pages_.begin();
    for(; iter != pages_.end(); ++iter)
    {
        PagePtr page = (*iter);
        if (page)
        {
            page->updateDisplaySize(region, page_layout_type, *this);
            // reset the position
            page->updateDisplayPosition(QPoint(0, 0));
        }
    }
}

void LayoutPages::updatePage(const int page_number,
                             const QSize &region,
                             const PageLayoutType page_layout_type)
{
    PagePtr page = getPage(page_number);
    if (page != 0)
    {
        page->updateDisplaySize(region, page_layout_type, *this);

        // reset the position
        page->updateDisplayPosition(QPoint(0, 0));
    }
}

void LayoutPages::resetVector(int new_page_number)
{
    if (new_page_number < static_cast<int>(pages_.size()))
    {
        return;
    }

    // make a new vector
    int new_size = new_page_number + vector_init_size;
    pages_.resize(new_size, 0);
}

// ----------------------------------------------------------------------------
PageLayout::PageLayout(RotateDegree rotation, ZoomFactor zoom_setting)
    : pages_cache_()
{
    pages_cache_.rotate(rotation);
    pages_cache_.setZoomSetting(zoom_setting);
}

PageLayout::~PageLayout()
{
}

void PageLayout::setPageToCache(int page_number, const QRect &actual_size)
{
    // construct a new page
    PagePtr new_page = new NumPage(page_number);
    new_page->setActualArea(actual_size);

    pages_cache_.setPage(new_page);
}

PagePtr PageLayout::getPage(const int key)
{
    return pages_cache_.getPage(key);
}

void PageLayout::setFirstPageNumber(const int first)
{
    pages_cache_.setFirstPageKey(first);
}

void PageLayout::setLastPageNumber(const int last)
{
    pages_cache_.setLastPageKey(last);
}

int PageLayout::getLastPageNumber()
{
    return pages_cache_.lastPageKey();
}

// Coordination transformation for hide margin --------------------------------
bool getDisplayContentPosition(const QRect & origin_content_area,
                               const QRect & origin_page_area,
                               const ZoomFactor zoom_value,
                               const RotateDegree rotate_degree,
                               QPoint & content_pos)
{
    if (!origin_content_area.isValid())
    {
        return false;
    }

    switch (rotate_degree)
    {
    case ROTATE_0_DEGREE:
        content_pos = origin_content_area.topLeft();
        break;
    case ROTATE_90_DEGREE:
        content_pos.setX(origin_page_area.height() - origin_content_area.bottom());
        content_pos.setY(origin_content_area.left());
        break;
    case ROTATE_270_DEGREE:
        content_pos.setX(origin_content_area.top());
        content_pos.setY(origin_page_area.width() - origin_content_area.right());
        break;
    default:
        break;
    }

    content_pos *= zoom_value;
    return true;
}

bool getDisplayContentAreas(const QRect & origin_content_area,
                            const QRect & origin_page_area,
                            const ZoomFactor zoom_value,
                            const RotateDegree rotate_degree,
                            QRect & content_area,
                            QRect & out_bounding_area)
{
    QPoint content_pos;
    if (!getDisplayContentPosition(origin_content_area,
                                   origin_page_area,
                                   zoom_value,
                                   rotate_degree,
                                   content_pos))
    {
        return false;
    }
    content_area.setTopLeft(content_pos);

    // get the size of out-bounding area
    QSize actual_out_bounding_size = origin_page_area.size();
    QSize content_size = origin_content_area.size();
    if (rotate_degree == ROTATE_90_DEGREE ||
        rotate_degree == ROTATE_270_DEGREE)
    {
        actual_out_bounding_size.transpose();
        content_size.transpose();
    }

    out_bounding_area.setSize(actual_out_bounding_size *= zoom_value);
    content_area.setSize(content_size *= zoom_value);
    return true;
}

bool getZoomValueByContentArea(const QSize & region_size,
                               const QSize & content_size,
                               const PageLayoutType page_layout_type,
                               const RotateDegree rotate_degree,
                               ZoomFactor & zoom_value)
{
    if (content_size.isEmpty() || !content_size.isValid())
    {
        return false;
    }

    ZoomFactor zoom_h = static_cast<ZoomFactor>(region_size.width()) /
                        static_cast<ZoomFactor>(content_size.width());
    ZoomFactor zoom_v = static_cast<ZoomFactor>(region_size.height()) /
                        static_cast<ZoomFactor>(content_size.height());

    zoom_value = page_layout_type == CONTINUOUS_LAYOUT ?
                 (rotate_degree == ROTATE_90_DEGREE ||
                  rotate_degree == ROTATE_270_DEGREE ? zoom_v : zoom_h) :
                  std::min(zoom_h, zoom_v);
    return true;
}

}  // namespace vbf
