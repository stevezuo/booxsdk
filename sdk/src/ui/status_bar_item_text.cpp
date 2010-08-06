#include "onyx/ui/status_bar_item_text.h"

namespace ui
{

StatusBarItemMessage::StatusBarItemMessage(QWidget *parent)
    : StatusBarItem(MESSAGE, parent)
{
    QFont f;
    f.setPointSize(20);
    f.setBold(true);
    setFont(f);
    metrics_.reset(new QFontMetrics(f));
}

StatusBarItemMessage::~StatusBarItemMessage(void)
{
}

void StatusBarItemMessage::setMessage(int current, int total)
{
    QString msg("%1/%2");
    msg = msg.arg(current).arg(total);
    setMessage(msg);
}

void StatusBarItemMessage::setMessage(const QString & msg)
{
    if (message_ == msg)
    {
        return;
    }

    bool update_layout = (metrics_->width(message_) != metrics_->width(msg));
    message_ = msg;
    update();

    if (update_layout)
    {
        updateGeometry();
    }
}

void StatusBarItemMessage::mousePressEvent(QMouseEvent *me)
{
    me->accept();
}

void StatusBarItemMessage::mouseMoveEvent(QMouseEvent *me)
{
    me->accept();
}

void StatusBarItemMessage::mouseReleaseEvent(QMouseEvent *me)
{
    me->accept();
    emit clicked();
}

void StatusBarItemMessage::paintEvent(QPaintEvent *pe)
{
    QPainter p(this);
    p.setPen(Qt::white);
    p.drawText(rect(), Qt::AlignCenter, message_);
}

QSize StatusBarItemMessage::sizeHint() const
{
    int w = metrics_->width(message_);
    int h = static_cast<int>(metrics_->height());
    return QSize(w, h);
}

QSize StatusBarItemMessage::minimumSizeHint() const
{
    return sizeHint();
}

}
