#include "onyx/base/device.h"
#include "onyx/ui/status_bar_item_clock.h"

namespace ui
{

StatusBarItemClock::StatusBarItemClock(QWidget *parent)
    : StatusBarItem(CLOCK, parent)
    , start_(QDateTime::currentDateTime())
{
    createLayout();
}

StatusBarItemClock::~StatusBarItemClock(void)
{
}

void StatusBarItemClock::createLayout()
{
    QImage & img = image();
    setFixedWidth(img.width());
}

void StatusBarItemClock::paintEvent(QPaintEvent *pe)
{
    QPainter painter(this);

    QImage & img = image();
    QPoint point;
    point.rx() = ((rect().width() - img.width()) >> 1);
    point.ry() = ((rect().height() - img.height()) >> 1);
    painter.drawImage(point, img);
}

void StatusBarItemClock::mousePressEvent(QMouseEvent *me)
{
    me->accept();
}

void StatusBarItemClock::mouseReleaseEvent(QMouseEvent *me)
{
    me->accept();
    emit clicked();
}

/// Retrieve image item according to battery value and status.
QImage & StatusBarItemClock::image()
{
    if (images_.isEmpty())
    {
        images_.insert(0, QImage(resourcePath()));
    }
    return images_[0];
}

QString StatusBarItemClock::resourcePath()
{
    QString str(":/images/clock.png");
    return str;
}

}
