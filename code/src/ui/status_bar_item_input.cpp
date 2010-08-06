#include "onyx/base/device.h"
#include "onyx/ui/status_bar_item_input.h"

namespace ui
{

StatusBarItemInput::StatusBarItemInput(QWidget *parent)
    : StatusBarItem(INPUT_TEXT, parent)
{
    createLayout();
}

StatusBarItemInput::~StatusBarItemInput(void)
{
}

void StatusBarItemInput::createLayout()
{
    QImage & img = image();
    setFixedWidth(img.width());
}

void StatusBarItemInput::paintEvent(QPaintEvent *pe)
{
    QPainter painter(this);

    QImage & img = image();
    QPoint point;
    point.rx() = ((rect().width() - img.width()) >> 1);
    point.ry() = ((rect().height() - img.height()) >> 1);
    painter.drawImage(point, img);
}

void StatusBarItemInput::mousePressEvent(QMouseEvent *me)
{
    me->accept();
}

void StatusBarItemInput::mouseReleaseEvent(QMouseEvent *me)
{
    me->accept();
    emit clicked();
}

QImage & StatusBarItemInput::image()
{
    if (images_.isEmpty())
    {
        images_.insert(0, QImage(resourcePath()));
    }
    return images_[0];
}

QString StatusBarItemInput::resourcePath()
{
    QString str(":/images/input_text.png");
    return str;
}

}
