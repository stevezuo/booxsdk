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
, context_()
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

void ThumbnailLayout::updateContext(const ThumbnailLayoutContext & other)
{
    context_ = other;
    updatePages();
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
    int thumb_width = (widget_.width() - (context_.columns + 1) * context_.space) / context_.columns;
    int thumb_height = (widget_.height() - (context_.rows + 1) * context_.space) / context_.rows;
    if (thumb_width <= 0 || thumb_height <= 0)
    {
        return;
    }

    pages_.clear();
    // calculate the size and position of each thumbnail
    int x = 0, y = 0;
    for (int i = 0; i < context_.rows; ++i)
    {
        y = i * thumb_height + (i + 1) * context_.space;
        for (int j = 0; j < context_.columns; ++j)
        {
            x = j * thumb_width + (j + 1) * context_.space;
            ThumbnailPage page;
            QRect rect(x, y, thumb_width, thumb_height);
            page.setThumb(rect, context_.txt_dir, context_.txt_ratio);

            pages_.push_back(page);
        }
    }
}

} // namespace ui
