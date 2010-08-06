#include "onyx/ui/status_bar_item_connection.h"

namespace ui
{

QString GetConnName(const int status)
{
    switch (status)
    {
    case 0:
        return ":/images/connection_0.png";
    case 1:
        return ":/images/connection_1.png";
    case 2:
        return ":/images/connection_2.png";
    case 3:
        return ":/images/connection_3.png";
    default:
        break;
    }
    return QString();
}

StatusBarItemConnection::StatusBarItemConnection(QWidget *parent)
    : StatusBarItem(CONNECTION, parent)
    , status_(-1)
{
    setConnectionStatus(0);
}

StatusBarItemConnection::~StatusBarItemConnection(void)
{
}

void StatusBarItemConnection::setConnectionStatus(const int status)
{
    if (status_ == status)
    {
        return;
    }
    status_ = status;

    QImage & img = image();
    setFixedWidth(img.width());
}

void StatusBarItemConnection::paintEvent(QPaintEvent *pe)
{
    QPainter painter(this);
    QImage & img = image();
    QPoint point;
    point.rx() = ((rect().width() - img.width()) >> 1);
    point.ry() = ((rect().height() - img.height()) >> 1);
    painter.drawImage(point, img);
}

/// Retrieve image item according to battery value and status.
QImage & StatusBarItemConnection::image()
{
    int key = status_;
    if (!images_.contains(key))
    {
        images_.insert(key, QImage(resourcePath()));
    }
    return images_[key];
}

QString StatusBarItemConnection::resourcePath()
{
    switch (status_)
    {
    case 0:
        return ":/images/power_0.png";
    case 1:
        return ":/images/power_1.png";
    case 2:
        return ":/images/power_2.png";
    case 3:
        return ":/images/power_3.png";
    case 4:
        return ":/images/power_4.png";
    default:
        break;
    }
    return QString();
}
}
