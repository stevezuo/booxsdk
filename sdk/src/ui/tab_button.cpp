#include <algorithm>
#include "onyx/ui/tab_button.h"

static const int SPACING = 5;

TabButton::TabButton(QWidget *parent, const int id)
    : QWidget(parent)
    , button_id_(id)
    , checked_(false)
    , is_dirty_(true)
{
    setAutoFillBackground(false);
}

TabButton::~TabButton()
{
}

QSize TabButton::sizeHint() const
{
    updateLayout();

    int w = std::max(iconActualSize().width(),
                     layout_width_) + 2 * SPACING;
    int h = iconActualSize().height() + layout_height_ + SPACING * 3;

    return QSize(w, h);
}

QSize TabButton::minimumSizeHint() const
{
    return sizeHint();
}

bool TabButton::event(QEvent *e)
{
    switch (e->type())
    {
    case QEvent::MouseMove:
    case QEvent::HoverMove:
    case QEvent::HoverEnter:
    case QEvent::HoverLeave:
        e->accept();
        return true;

    default:
        break;
    }
    return QWidget::event(e);
}

void TabButton::setText(const QString &title)
{
    if (title_layout_.text() != title)
    {
        is_dirty_ = true;
        title_layout_.setText(title);
    }
}

void TabButton::setIcon(const QIcon & icon)
{
    icon_ = icon;
    is_dirty_ = true;
}

void TabButton::setChecked(bool checked)
{
    if (checked_ != checked)
    {
        checked_ = checked;
        update();
    }
}

/// TODO, change it to better rendering.
void TabButton::paintEvent(QPaintEvent *e)
{
    updateLayout();
    QPainter p(this);

    static const int OFFSET = 1;
    static const int ROUNDED = 8;
    QPainterPath path;
    QRect rc = rect();
    rc.adjust(OFFSET, OFFSET, -OFFSET, -OFFSET);
    path.addRoundedRect(rc, ROUNDED, ROUNDED, Qt::AbsoluteSize);

    QPen pen(Qt::SolidLine);
    if (isChecked())
    {
        p.fillPath(path, QBrush(QColor(0, 0, 0, 255)));
        pen.setColor(QColor(255, 255, 255, 255));
    }
    else
    {
        p.fillPath(path, QBrush(QColor(128, 128, 128, 128)));
        pen.setColor(QColor(0, 0, 0, 255));
    }
    p.setPen(pen);

    // Draw text.
    title_layout_.draw(&p, QPoint());

    // Draw icon.
    icon().paint(&p,
                 QRect(icon_pos_, iconActualSize()),
                 Qt::AlignTop|Qt::AlignHCenter);

    if (hasFocus())
    {
        pen.setColor(QColor(255, 255, 255, 255));
    }
    else
    {
        pen.setColor(QColor(0, 0, 0, 255));
    }

    // Draw border.
    p.setPen(pen);
    p.drawPath(path);
}

void TabButton::mousePressEvent(QMouseEvent *e)
{
}

void TabButton::mouseReleaseEvent(QMouseEvent *e)
{
    setChecked(true);
    setFocus();
    emit clicked(id(), true);
}

void TabButton::click()
{
    setChecked(true);
    setFocus();
    emit clicked(id(), true);
}

void TabButton::updateLayout() const
{
    if (is_dirty_)
    {
        title_layout_.setCacheEnabled(true);
        title_layout_.beginLayout();
        QTextLine line = title_layout_.createLine();
        if (line.isValid())
        {
            line.setLineWidth(rect().width());
        }
        title_layout_.endLayout();
        layout_width_ = static_cast<int>(line.naturalTextRect().width());
        layout_height_ = static_cast<int>(line.height());
    }

    int x = ((rect().width() - layout_width_) >> 1);
    title_layout_.setPosition(QPoint(x, iconActualSize().height() + 2 * SPACING));

    // icon.
    icon_pos_.rx() = ((rect().width() - iconActualSize().width()) >> 1);
    icon_pos_.ry() = SPACING;

    is_dirty_ = false;
}

QSize TabButton::iconActualSize() const
{
    return icon().actualSize(QSize(1024, 1024));
}
