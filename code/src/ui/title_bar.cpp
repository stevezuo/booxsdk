
#include <QtGui/QtGui>
#include "onyx/ui/title_bar.h"
#include "onyx/ui/text_layout.h"

namespace ui
{

OnyxTitleBar::OnyxTitleBar(const QImage & image, QWidget *parent)
    : QWidget(parent)
    , font_(QApplication::font())
    , image_(image)
    , layout_dirty_(true)
{
    font_.setPointSize(20);
    setFixedHeight(40);
}

OnyxTitleBar::~OnyxTitleBar()
{
}

void OnyxTitleBar::setTitle(const QString & title)
{
    if (title != text_layout_.text())
    {
        text_layout_.setText(title);
        layout_dirty_ = true;
    }
}

QString OnyxTitleBar::title() const
{
    return text_layout_.text();
}

void OnyxTitleBar::updateLayout()
{
    if (layout_dirty_)
    {
        static const int SPACING = 10;
        image_pos_.rx() = SPACING ;
        image_pos_.ry() = ((rect().height() - image_.height()) >> 1);
        QRect text_rc(rect());
        text_rc.adjust(image_pos_.x() + image_.width() + SPACING, 0, 0, 0);
        ui::calculateSingleLineLayout(text_layout_,
                                      font_,
                                      text_layout_.text(),
                                      Qt::AlignLeft|Qt::AlignVCenter,
                                      text_rc);
        layout_dirty_ = false;
    }
}

void OnyxTitleBar::paintEvent(QPaintEvent *pe)
{
    updateLayout();

    QPainter p(this);
    p.fillRect(rect(), QBrush(QColor(0, 0, 0)));

    p.drawImage(image_pos_, image_);

    QPen pen(QColor(255, 255, 255));
    p.setPen(pen);
    text_layout_.draw(&p, QPoint(0, 0));
}

}
