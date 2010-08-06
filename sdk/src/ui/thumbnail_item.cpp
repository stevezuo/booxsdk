#include "onyx/ui/thumbnail_item.h"
#include "onyx/screen/screen_proxy.h"

namespace ui
{

static QPoint getImageDisplayPosition(const QRect & bounding_rect, const QSize & img_size)
{
    QPoint pos(-1, -1);
    if (bounding_rect.width() < (img_size.width() - 5) ||
        bounding_rect.height() < (img_size.height() - 5))
    {
        // if the image area is larger than the bounding rectangle, return invalid position
        qWarning("Invalid Thumbnail Size");
    }
    else
    {
        pos.setX(bounding_rect.left() + ((bounding_rect.width() - img_size.width()) >> 1));
        pos.setY(bounding_rect.top() + ((bounding_rect.height() - img_size.height()) >> 1));
    }
    return pos;
}

ImageItem::ImageItem(QWidget * parent)
: QWidget(parent, Qt::FramelessWindowHint)
{
}

ImageItem::~ImageItem()
{
}

void ImageItem::setImage(const QImage & img, bool fast_update)
{
    image_ = img;
    if (fast_update)
    {
        onyx::screen::instance().enableUpdate(false);
        repaint();
        onyx::screen::instance().enableUpdate(true);
        onyx::screen::instance().updateWidget(this, onyx::screen::ScreenProxy::GU);
    }
}

void ImageItem::clear()
{
}

void ImageItem::paintEvent(QPaintEvent * pe)
{
    if (image_.isNull())
    {
        return;
    }

    QPainter painter(this);

    if (image_.width() > width() || image_.height() > height())
    {
        //QSize size_limit = thumbnailSize(THUMBNAIL_HUGE);
        //image_ = image_.scaled(size_limit, Qt::KeepAspectRatio);
        image_ = image_.scaled(size(), Qt::KeepAspectRatio);
    }

    QPoint pos = getImageDisplayPosition( rect(), image_.size() );
    if (pos.x() < 0 || pos.y() < 0)
    {
        return;
    }

    painter.drawImage(pos, image_);
}

// Thumbnail Item
ThumbnailItem::ThumbnailItem(QWidget * parent)
    : image_(parent)
    , text_(parent)
{
    createLayout();
}

ThumbnailItem::~ThumbnailItem()
{
}

QRect ThumbnailItem::imageRect()
{
    return image_.geometry();
}

void ThumbnailItem::setImage(const QImage & img, bool fast_update)
{
    image_.setImage(img, fast_update);
}

void ThumbnailItem::setText(const QString & text)
{
    text_.setText(text);
}

QRect ThumbnailItem::rect()
{
    return image_.geometry().united(text_.geometry());
}

void ThumbnailItem::createLayout()
{
    image_.setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    vbox_.addWidget(&image_);

    text_.setWordWrap(true);
    text_.setAlignment(Qt::AlignCenter);
    text_.setFixedHeight(35);
    text_.setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    vbox_.addWidget(&text_);
}

QLayout* ThumbnailItem::layout()
{
    return &vbox_;
}

}
