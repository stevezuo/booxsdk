#include "onyx/base/device.h"
#include "onyx/ui/status_bar_item_refresh_screen.h"

namespace ui
{

StatusBarItemRefreshScreen::StatusBarItemRefreshScreen(QWidget *parent)
    : StatusBarItem(SCREEN_REFRESH, parent)
{
    createLayout();
}

StatusBarItemRefreshScreen::~StatusBarItemRefreshScreen(void)
{
}

void StatusBarItemRefreshScreen::createLayout()
{
    QImage & img = image();
    setFixedWidth(img.width());
}

void StatusBarItemRefreshScreen::paintEvent(QPaintEvent *pe)
{
    QPainter painter(this);

    QImage & img = image();
    QPoint point;
    point.rx() = ((rect().width() - img.width()) >> 1);
    point.ry() = ((rect().height() - img.height()) >> 1);
    painter.drawImage(point, img);
}

void StatusBarItemRefreshScreen::mousePressEvent(QMouseEvent *me)
{
    me->accept();
}

void StatusBarItemRefreshScreen::mouseReleaseEvent(QMouseEvent *me)
{
    me->accept();
    emit clicked();
}

QImage & StatusBarItemRefreshScreen::image()
{
    if (images_.isEmpty())
    {
        images_.insert(0, QImage(resourcePath()));
    }
    return images_[0];
}

QString StatusBarItemRefreshScreen::resourcePath()
{
    QString str(":/images/refresh_screen.png");
    return str;
}

}
