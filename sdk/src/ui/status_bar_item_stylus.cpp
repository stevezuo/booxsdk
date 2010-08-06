#include "onyx/ui/status_bar_item_stylus.h"

namespace ui
{

StatusBarItemStylus::StatusBarItemStylus(QWidget *parent)
    : StatusBarItem(STYLUS, parent)
{
    setStylusState(ID_UNKNOWN);
}

StatusBarItemStylus::~StatusBarItemStylus(void)
{
}

void StatusBarItemStylus::setStylusState(const int s)
{
    if (s < ID_ZOOM_IN || s >= ID_UNKNOWN)
    {
        qDebug("Invalid Stylus Type:%d", s);
        return;
    }

    if (state() == s)
    {
        return;
    }
    setState(s);
    setFixedWidth(image().width());
    update();
}

void StatusBarItemStylus::paintEvent(QPaintEvent *pe)
{
    QPainter painter(this);
    QImage & img = image();
    QPoint point;
    point.rx() = ((rect().width() - img.width()) >> 1);
    point.ry() = ((rect().height() - img.height()) >> 1);
    painter.drawImage(point, img);
}

QImage & StatusBarItemStylus::image()
{
    int key = state();
    if (!images_.contains(key))
    {
        images_.insert(key, QImage(resourcePath()));
    }
    return images_[key];
}

QString StatusBarItemStylus::resourcePath()
{
    QString name;
    switch (state())
    {
    case ID_PAN:
        name = ":/images/stylus_pan.png";
        break;
    case ID_SKETCHING:
        name = ":/images/stylus_sketch.png";
        break;
    case ID_ZOOM_IN:
        name = ":/images/stylus_zoom_in.png";
        break;
    case ID_ERASING:
        name = ":/images/stylus_erase_sketch.png";
        break;
    case ID_ADD_ANNOTATION:
        name = ":/images/stylus_add_annotation.png";
        break;
    case ID_DELETE_ANNOTATION:
        name = ":/images/stylus_erase_annotation.png";
        break;
    case ID_FREE_PEN:
        name = ":/images/stylus_pointer.png";
        break;
    default:
        name = ":/images/stylus_pan.png";
        break;
    }
    return name;
}

}
