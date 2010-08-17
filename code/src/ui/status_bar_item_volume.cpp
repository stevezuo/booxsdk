#include "onyx/base/device.h"
#include "onyx/ui/status_bar_item_volume.h"

namespace ui
{

StatusBarItemVolume::StatusBarItemVolume(QWidget *parent)
    : StatusBarItem(VOLUME, parent)
{
    createLayout();
}

StatusBarItemVolume::~StatusBarItemVolume(void)
{
}

void StatusBarItemVolume::createLayout()
{
    QImage & img = image();
    setFixedWidth(img.width());
}

void StatusBarItemVolume::paintEvent(QPaintEvent *pe)
{
    QPainter painter(this);

    QImage & img = image();
    QPoint point;
    point.rx() = ((rect().width() - img.width()) >> 1);
    point.ry() = ((rect().height() - img.height()) >> 1);
    painter.drawImage(point, img);
}

void StatusBarItemVolume::mousePressEvent(QMouseEvent *me)
{
    me->accept();
}

void StatusBarItemVolume::mouseReleaseEvent(QMouseEvent *me)
{
    me->accept();
    emit clicked();
}

/// Retrieve image item according to battery value and status.
QImage & StatusBarItemVolume::image()
{
    if (images_.isEmpty())
    {
        images_.insert(0, QImage(resourcePath()));
    }
    return images_[0];
}

QString StatusBarItemVolume::resourcePath()
{
    QString str(":/images/volume.png");
    return str;
}

}
