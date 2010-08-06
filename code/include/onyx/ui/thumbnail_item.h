#ifndef ONYX_THUMBNAIL_ITEM_H__
#define ONYX_THUMBNAIL_ITEM_H__

#include "onyx/base/base.h"
#include "ui_global.h"

namespace ui
{

enum
{
    THUMB_SPACING = 8
};

class ImageItem : public QWidget
{
    Q_OBJECT
public:
    ImageItem(QWidget * parent = 0);
    ~ImageItem();

    void setImage(const QImage & img, bool fast_update);
    void clear();

protected:
    // message handlers
    void paintEvent(QPaintEvent *pe);

private:
    QImage image_;
};

class ThumbnailItem
{
public:
    ThumbnailItem(QWidget * parent = 0);
    ~ThumbnailItem();

    void setImage(const QImage & img, bool fast_update);
    void setText(const QString & text);
    QRect imageRect();
    QRect rect();
    QLayout* layout();

private:
    void createLayout();

private:
    QVBoxLayout             vbox_;
    ImageItem               image_;
    QLabel                  text_;
};

}; // namespace ui

#endif  // ONYX_THUMBNAIL_ITEM_H__
