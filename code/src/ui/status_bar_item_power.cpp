#include "onyx/base/device.h"
#include "onyx/ui/status_bar_item_power.h"

namespace ui
{

static int index(int value)
{
    int key = value / 20;
    if (key < 0)
    {
        key = 0;
    }
    else if (key > 5)
    {
        key = 5;
    }
    return key;
}

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

    int old_index = index(value_);
    int new_index = index(value);
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
    int key = index(value_);
    if (status_ & BATTERY_STATUS_CHARGING)
    {
        key |= (1 << 16);
    }

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
        str = str.arg(index(value_));
        return str;
    }
    else
    {
        QString str(":/images/battery_%1.png");
        str = str.arg(index(value_));
        return str;
    }
    return QString();
}

}
