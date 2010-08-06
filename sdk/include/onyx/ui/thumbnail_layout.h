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

class ThumbnailLayout
{
public:
    ThumbnailLayout();
    ~ThumbnailLayout();

    void setWidgetSize(const QSize &s);

    void setRow(const int r);
    int  numRow();

    void setColumn(const int c);
    int  numColumn();

    void setSpace(const int s);
    int  space();

    bool hitTest(const QPoint &pos, int &page);

    void setTextRatio(const float r);
    void setTextDirection(const TextDirection d);
    TextDirection textDirection();

    ThumbnailPages& pages();

private:
    ThumbnailPages    pages_;
    int               rows_;
    int               columns_;
    int               space_;
    TextDirection     txt_dir_;
    float             txt_ratio_;

    QSize             widget_;

private:
    void updatePages();
};

}; // namespace ui

#endif  // ONYX_THUMBNAIL_LAYOUT_H__
