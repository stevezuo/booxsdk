#ifndef ONYX_THUMBNAIL_LAYOUT_H__
#define ONYX_THUMBNAIL_LAYOUT_H__

#include "onyx/base/base.h"
#include "ui_global.h"

namespace ui
{

enum TextDirection
{
    TEXT_BOTTOM = 0,
    TEXT_TOP,
    TEXT_LEFT,
    TEXT_RIGHT
};

class ThumbnailPage
{
public:
    QRect    image_area;
    QRect    text_area;

public:
    ThumbnailPage() : image_area(), text_area() {}
    ~ThumbnailPage() {}

    void setThumb(const QRect &rect, const TextDirection d, const float r);
};

typedef QVector<ThumbnailPage> ThumbnailPages;
typedef ThumbnailPages::iterator ThumbnailPagesIter;

struct ThumbnailLayoutContext
{
    int               rows;
    int               columns;
    int               space;
    TextDirection     txt_dir;
    float             txt_ratio;

    ThumbnailLayoutContext() : rows(2), columns(2), space(10), txt_dir(TEXT_BOTTOM), txt_ratio(0.1f) {}
};

class ThumbnailLayout
{
public:
    ThumbnailLayout();
    ~ThumbnailLayout();

    void setWidgetSize(const QSize &s);
    const ThumbnailLayoutContext & context() const { return context_; }
    void updateContext(const ThumbnailLayoutContext & other);
    bool hitTest(const QPoint &pos, int &page);

    ThumbnailPages& pages();

private:
    void updatePages();

private:
    ThumbnailPages          pages_;
    ThumbnailLayoutContext  context_;
    QSize                   widget_;
};

}; // namespace ui

#endif  // ONYX_THUMBNAIL_LAYOUT_H__
