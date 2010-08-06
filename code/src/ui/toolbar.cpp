
#include <QtGui/QtGui>
#include "onyx/ui/toolbar.h"
#include "onyx/screen/screen_proxy.h"

static const int STATE_NORMAL = 0;
static const int STATE_PRESSED = 1;

namespace ui
{

OnyxToolBarItem::OnyxToolBarItem(QWidget *parent, QAction &action)
: QWidget(parent)
, action_(action)
, state_(STATE_NORMAL)
{
    connect(&action_, SIGNAL(changed()), this, SLOT(onActionChanged()));
    QSize size = iconSize();
    size += QSize(2, 2);
    setFixedSize(size);
    setAutoFillBackground(false);
}

OnyxToolBarItem::~OnyxToolBarItem()
{
}

void OnyxToolBarItem::onActionChanged()
{
    update();
}

QSize OnyxToolBarItem::iconSize()
{
    return QSize(28, 28);
}

void OnyxToolBarItem::mousePressEvent(QMouseEvent *me)
{
    me->accept();
    if (!action_.isEnabled())
    {
        return;
    }

    state_ = STATE_PRESSED;
    onyx::screen::instance().enableUpdate(false);
    repaint();
    onyx::screen::instance().updateWidget(this, onyx::screen::ScreenProxy::DW, false);
    onyx::screen::instance().enableUpdate(true);
}

void OnyxToolBarItem::mouseReleaseEvent(QMouseEvent *me)
{
    me->accept();
    if (!action_.isEnabled())
    {
        return;
    }

    action_.trigger();
    state_ = STATE_NORMAL;
    onyx::screen::instance().enableUpdate(false);
    repaint();
    onyx::screen::instance().enableUpdate(true);
}

void OnyxToolBarItem::paintEvent(QPaintEvent *pe)
{
    QPainter p(this);
    if (state_ == STATE_PRESSED)
    {
        p.fillRect(rect(), Qt::black);
    }

    QRect icon_rect(QPoint(1, 1), iconSize());
    if (action_.isEnabled())
    {
        action_.icon().paint(&p, icon_rect, Qt::AlignCenter, QIcon::Normal, QIcon::On);
    }
    else
    {
        action_.icon().paint(&p, icon_rect, Qt::AlignCenter, QIcon::Disabled, QIcon::On);
    }
}


OnyxToolBar::OnyxToolBar(QWidget *parent)
    : QWidget(parent)
    , layout_(this)
{
    setAutoFillBackground(false);
    setFixedHeight(35);
    layout_.setSpacing(8);
    layout_.setContentsMargins(2, 1, 2, 1);
    layout_.insertStretch(0, 0);
}

OnyxToolBar::~OnyxToolBar()
{
}

void OnyxToolBar::addAction(QAction *action)
{
    OnyxToolBarItemPtr ptr(new OnyxToolBarItem(this, *action));
    items_.push_back(ptr);
    layout_.insertWidget(items_.size() - 1, ptr.get());
}

bool OnyxToolBar::event(QEvent *e)
{
    switch (e->type())
    {
    //case QEvent::MouseMove:
    case QEvent::HoverMove:
    case QEvent::HoverEnter:
    case QEvent::HoverLeave:
    case QEvent::Enter:
    case QEvent::Leave:
        e->accept();
        return true;
    default:
        break;
    }
    return QWidget::event(e);
}

void OnyxToolBar::paintEvent(QPaintEvent *pe)
{
    QPainter painter(this);
    QRect rc(rect());

    painter.fillRect(rc, QBrush(QColor(255, 255, 255)));
    //painter.setPen(QColor(255, 255, 255));
    //painter.drawLine(rc.left(), rc.bottom() - 1, rc.right(), rc.bottom() - 1);

    painter.setPen(QColor(0, 0, 0));
    painter.drawLine(rc.left(), rc.bottom(), rc.right(), rc.bottom());
}

}
