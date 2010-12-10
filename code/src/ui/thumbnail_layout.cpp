#include "onyx/ui/thumbnail_layout.h"

namespace ui
{

void ThumbnailPage::setThumb(const QRect &rect,
                             const TextDirection d,
                             const float r)
{
    switch (d)
    {
    case TEXT_BOTTOM:
        {
            int text_height = static_cast<int>(rect.height() * r);
            text_area.setHeight(text_height);
            text_area.setWidth(rect.width());

            image_area.setHeight(rect.height() - text_height);
            image_area.setWidth(rect.width());

            image_area.moveTopLeft(rect.topLeft());
            text_area.moveLeft(rect.left());
            text_area.moveTop(rect.bottom() - text_height);
        }

        break;
    case TEXT_TOP:
        {
            int text_height = static_cast<int>(rect.height() * r);
            text_area.setHeight(text_height);
            text_area.setWidth(rect.width());

            image_area.setHeight(rect.height() - text_height);
            image_area.setWidth(rect.width());

            text_area.moveTopLeft(rect.topLeft());
            image_area.moveLeft(rect.left());
            image_area.moveTop(rect.top() + text_height);
        }

        break;
    case TEXT_LEFT:
        {
            int text_width = static_cast<int>(rect.width() * r);
            text_area.setHeight(rect.height());
            text_area.setWidth(text_width);

            image_area.setHeight(rect.height());
            image_area.setWidth(rect.width() - text_width);

            text_area.moveTopLeft(rect.topLeft());
            image_area.moveLeft(rect.left() + text_width);
            image_area.moveTop(rect.top());
        }

        break;
    case TEXT_RIGHT:
        {
            int text_width = static_cast<int>(rect.width() * r);
            text_area.setHeight(rect.height());
            text_area.setWidth(text_width);

            image_area.setHeight(rect.height());
            image_area.setWidth(rect.width() - text_width);

            image_area.moveTopLeft(rect.topLeft());
            text_area.moveLeft(rect.right() - text_width);
            text_area.moveTop(rect.top());
        }

        break;
    }
}

ThumbnailLayout::ThumbnailLayout()
: pages_()
, rows_(2)
, columns_(2)
, space_(10)
, txt_dir_(TEXT_BOTTOM)
, txt_ratio_(0.1f)
, widget_()
{
}

ThumbnailLayout::~ThumbnailLayout()
{
}

void ThumbnailLayout::setWidgetSize(const QSize &s)
{
    widget_ = s;
    updatePages();
}

void ThumbnailLayout::setRow(const int r)
{
    rows_ = r;
}

int  ThumbnailLayout::numRow()
{
    return rows_;
}

void ThumbnailLayout::setColumn(const int c)
{
    columns_ = c;
}

int  ThumbnailLayout::numColumn()
{
    return columns_;
}

void ThumbnailLayout::setSpace(const int s)
{
    space_ = s;
    updatePages();
}

int  ThumbnailLayout::space()
{
    return space_;
}

void ThumbnailLayout::setTextRatio(const float r)
{
    txt_ratio_ = r;
    updatePages();
}

void ThumbnailLayout::setTextDirection(const TextDirection d)
{
    txt_dir_ = d;
    updatePages();
}

TextDirection ThumbnailLayout::textDirection()
{
    return txt_dir_;
}

ThumbnailPages& ThumbnailLayout::pages()
{
    return pages_;
}

bool ThumbnailLayout::hitTest(const QPoint &pos, int &page)
{
    // TODO. Coordination Transform
    bool ret = false;
    ThumbnailPagesIter idx = pages_.begin();
    int page_idx = 0;
    for (; idx != pages_.end(); ++idx)
    {
        if (idx->image_area.contains(pos) || idx->text_area.contains(pos))
        {
            ret = true;
            page = page_idx;
            break;
        }

        page_idx++;
    }

    return ret;
}

void ThumbnailLayout::updatePages()
{
    assert(widget_.isValid());

    // calculate the size of each thumbnail
    int thumb_width = (widget_.width() - (columns_ + 1) * space_) / columns_;
    int thumb_height = (widget_.height() - (rows_ + 1) * space_) / rows_;
    if (thumb_width <= 0 || thumb_height <= 0)
    {
        return;
    }

    pages_.clear();
    // calculate the size and position of each thumbnail
    int x = 0, y = 0;
    for (int i = 0; i < rows_; ++i)
    {
        y = i * thumb_height + (i + 1) * space_;
        for (int j = 0; j < columns_; ++j)
        {
            x = j * thumb_width + (j + 1) * space_;
            ThumbnailPage page;
            QRect rect(x, y, thumb_width, thumb_height);
            page.setThumb(rect, txt_dir_, txt_ratio_);

            pages_.push_back(page);
        }
    }
}

} // namespace ui
