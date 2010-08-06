#include "onyx/base/device.h"
#include "onyx/ui/status_bar_item_power.h"

namespace ui
{

StatusBarItemBattery::StatusBarItemBattery(QWidget *parent)
    : StatusBarItem(BATTERY, parent)
    , value_(100)
    , status_(0)
{
    createLayout();
}

StatusBarItemBattery::~StatusBarItemBattery(void)
{
}

void StatusBarItemBattery::createLayout()
{
    QImage & img = image();
    setFixedWidth(img.width());
}

bool StatusBarItemBattery::setBatteryStatus(const int value,
                                            const int status)
{
    if (value_ == value && status_ == status)
    {
        return false;
    }

    int old_index = (100 - value_) / 20;
    int new_index = (100 - value) / 20;
    bool status_changed = (status_ != status);

    value_ = value;
    status_ = status;

    if (old_index != new_index || status_changed)
    {
        update();
        return true;
    }
    return false;
}

void StatusBarItemBattery::paintEvent(QPaintEvent *pe)
{
    QPainter painter(this);

    QImage & img = image();
    QPoint point;
    point.rx() = ((rect().width() - img.width()) >> 1);
    point.ry() = ((rect().height() - img.height()) >> 1);
    painter.drawImage(point, img);
}

void StatusBarItemBattery::mousePressEvent(QMouseEvent *me)
{
    me->accept();
}

void StatusBarItemBattery::mouseReleaseEvent(QMouseEvent *me)
{
    me->accept();
    emit clicked();
}

/// Retrieve image item according to battery value and status.
QImage & StatusBarItemBattery::image()
{
    int key = status_ & BATTERY_STATUS_CHARGING;
    key = (key << 8) | ((100 - value_) / 20);
    if (!images_.contains(key))
    {
        images_.insert(key, QImage(resourcePath()));
    }
    return images_[key];
}

QString StatusBarItemBattery::resourcePath()
{
    if (status_ & BATTERY_STATUS_CHARGING)
    {
        QString str(":/images/battery_charge_%1.png");
        str = str.arg((100 - value_) / 20);
        return str;
    }
    else
    {
        QString str(":/images/battery_%1.png");
        str = str.arg((100 - value_) / 20);
        return str;
    }
    return QString();
}

}
