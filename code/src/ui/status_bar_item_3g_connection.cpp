#include "onyx/ui/status_bar_item_3g_connection.h"
#include "onyx/sys/sys_status.h"

namespace ui
{

StatusBarItem3GConnection::StatusBarItem3GConnection(QWidget *parent)
    : StatusBarItem(CONNECTION, parent)
    , status_(-1)
{
    setConnectionStatus(0);
    connect(&sys::SysStatus::instance(),
            SIGNAL(report3GNetwork(const int, const int, const int)),
            this,
            SLOT(onSignalStrengthChanged(const int, const int, const int)));
}

StatusBarItem3GConnection::~StatusBarItem3GConnection(void)
{
}

void StatusBarItem3GConnection::setConnectionStatus(const int status)
{
    if (status_ == status)
    {
        return;
    }
    status_ = status;

    QImage & img = image();
    setFixedWidth(img.width());
}

void StatusBarItem3GConnection::paintEvent(QPaintEvent *pe)
{
    QPainter painter(this);
    QImage & img = image();
    QPoint point;
    point.rx() = ((rect().width() - img.width()) >> 1);
    point.ry() = ((rect().height() - img.height()) >> 1);
    painter.drawImage(point, img);
}

/// Retrieve image item according to battery value and status.
QImage & StatusBarItem3GConnection::image()
{
    int key = status_;
    if (!images_.contains(key))
    {
        images_.insert(key, QImage(resourcePath()));
    }
    return images_[key];
}

QString StatusBarItem3GConnection::resourcePath()
{
    switch (status_)
    {
    case 0:
        return ":/images/signal_0.png";
    case 1:
        return ":/images/signal_1.png";
    case 2:
        return ":/images/signal_2.png";
    case 3:
        return ":/images/signal_3.png";
    case 4:
        return ":/images/signal_4.png";
    case 5:
        return ":/images/signal_5.png";
    default:
        return ":/images/signal_x.png";
        break;
    }
    return QString();
}

void StatusBarItem3GConnection::onSignalStrengthChanged(const int signal,
                                                        const int total,
                                                        const int network)
{
    qDebug("signal:%d total:%d network:%d",signal,total,network);

    int strength = signal;
    if (strength > 5)
    { 
        strength = 5;
    }

    setConnectionStatus(strength);
}

}
