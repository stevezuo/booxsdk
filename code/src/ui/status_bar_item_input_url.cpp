#include "onyx/base/device.h"
#include "onyx/ui/status_bar_item_input_url.h"

namespace ui
{

StatusBarItemInputUrl::StatusBarItemInputUrl(QWidget *parent)
    : StatusBarItem(INPUT_URL, parent)
{
    createLayout();
}

StatusBarItemInputUrl::~StatusBarItemInputUrl(void)
{
}

void StatusBarItemInputUrl::createLayout()
{
    QImage & img = image();
    setFixedWidth(img.width());
}

void StatusBarItemInputUrl::paintEvent(QPaintEvent *pe)
{
    QPainter painter(this);

    QImage & img = image();
    QPoint point;
    point.rx() = ((rect().width() - img.width()) >> 1);
    point.ry() = ((rect().height() - img.height()) >> 1);
    painter.drawImage(point, img);
}

void StatusBarItemInputUrl::mousePressEvent(QMouseEvent *me)
{
    me->accept();
}

void StatusBarItemInputUrl::mouseReleaseEvent(QMouseEvent *me)
{
    me->accept();
    emit clicked();
}

QImage & StatusBarItemInputUrl::image()
{
    if (images_.isEmpty())
    {
        images_.insert(0, QImage(resourcePath()));
    }
    return images_[0];
}

QString StatusBarItemInputUrl::resourcePath()
{
    QString str(":/images/input_url.png");
    return str;
}

}
