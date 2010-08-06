#include "onyx/wireless/ap_item.h"
#include "onyx/screen/screen_proxy.h"

namespace ui
{

const QString TYPE_LABEL_STYLE = "      \
QLabel                                  \
{                                       \
     padding: 0px;                      \
     background: transparent;           \
     font: 24px ;                       \
     color: black;                      \
 }";

/// Compiler issue.
#ifndef _WINDOWS
const int WifiAPItem::SIGNAL_ICONS;
#endif

static const int SPACING = 20;
static const int ICON_SIZE = 44 + SPACING;
static const QColor TITLE_BK_COLOR = QColor(210, 210, 210);
static const QColor NORMAL_BK_COLOR = QColor(255, 255, 255);
static const QColor SELECTED_BK_COLOR = QColor(164, 164, 164);


WifiTitleItem::WifiTitleItem(QWidget *parent)
: QWidget(parent)
, layout_(this)
, title_label_(tr("Starting Wifi Device..."), 0)
, customize_button_(QPixmap(":/images/customize.png"), "", 0)
, refresh_button_(QPixmap(":/images/refresh.png"), "", 0)
{
    setAutoFillBackground(false);
    setFixedHeight(80);
    createLayout();
}

WifiTitleItem::~WifiTitleItem()
{
}

void WifiTitleItem::createLayout()
{
    layout_.setContentsMargins(SPACING, SPACING / 10, SPACING, SPACING / 10);
    layout_.setSpacing(10);

    layout_.addWidget(&title_label_, 0, Qt::AlignVCenter);
    layout_.addStretch(0);
    layout_.addWidget(&customize_button_);
    layout_.addWidget(&refresh_button_, 0, Qt::AlignVCenter);

    customize_button_.setFocusPolicy(Qt::NoFocus);
    refresh_button_.setFocusPolicy(Qt::NoFocus);

    // Retransmit signal.
    connect(&customize_button_, SIGNAL(clicked(bool)), this, SLOT(onCustomizedClicked(bool)));
    connect(&refresh_button_, SIGNAL(clicked(bool)), SIGNAL(refreshClicked()));
}

void WifiTitleItem::setState(const QString & state)
{
    title_label_.setText(state);
}

void WifiTitleItem::onCustomizedClicked(bool)
{
    emit customizedClicked();
}



bool WifiTitleItem::event(QEvent *e)
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

scoped_ptr<QPixmap> WifiAPItem::selected_pixmap_;
WifiAPItem *WifiAPItem::selected_item_ = 0;

WifiAPItem::WifiAPItem(QWidget *parent)
    : QWidget(parent)
    , hor_layout_(this)
    , ssid_label_(0)
    , config_button_(QPixmap(":/images/customize.png"), "", 0)
    , lock_icon_label_(0)
{
    createLayout();
    updateByProfile(profile_);
}

WifiAPItem::~WifiAPItem(void)
{
}

void WifiAPItem::setProfile(WifiProfile & profile)
{
    if (!(profile_ == profile))
    {
        profile_ = profile;
        updateByProfile(profile_);
    }
    selected_item_ = 0;
}

WifiProfile & WifiAPItem::profile()
{
    return profile_;
}

bool WifiAPItem::event(QEvent *e)
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

void WifiAPItem::paintEvent(QPaintEvent *e)
{
    if (profile_.bssid().isEmpty())
    {
        return;
    }

    QPainter painter(this);
    QPainterPath path;
    path.addRoundedRect(rect().adjusted(2, 2, -2, -2), 8, 8, Qt::AbsoluteSize);

    if (isSelected())
    {
        painter.fillPath(path, QBrush(SELECTED_BK_COLOR));
    }
    else
    {
        painter.fillPath(path, QBrush(NORMAL_BK_COLOR));
    }

    if (isSelected())
    {
        if (!selected_pixmap_)
        {
            selected_pixmap_.reset(new QPixmap(":/images/selected.png"));
        }
        painter.drawPixmap(SPACING, (height() - selected_pixmap_->height()) / 2, *selected_pixmap_);
    }

    painter.setPen(QColor(Qt::black));
    painter.drawPath(path);
}

void WifiAPItem::mousePressEvent(QMouseEvent *e)
{
    if (!profile_.bssid().isEmpty())
    {
        selected_item_ = this;
        onyx::screen::instance().enableUpdate(false);
        repaint();
        onyx::screen::instance().enableUpdate(true);
        onyx::screen::instance().updateWidget(this, onyx::screen::ScreenProxy::GU, true);
    }
}

void WifiAPItem::mouseReleaseEvent(QMouseEvent *e)
{
    if (!profile_.bssid().isEmpty())
    {
        emit clicked(profile_);
    }
}

void WifiAPItem::createLayout()
{
    hor_layout_.setContentsMargins(ICON_SIZE + SPACING, SPACING / 2, SPACING, SPACING / 2);
    hor_layout_.setSpacing(0);

    ssid_label_.setContentsMargins(0, 0, 0, 0);
    hor_layout_.addWidget(&ssid_label_, 0, Qt::AlignVCenter);
    hor_layout_.addStretch(0);
    hor_layout_.addWidget(&lock_icon_label_, 0, Qt::AlignBottom);
    for(int i = 0; i < SIGNAL_ICONS; ++i)
    {
        hor_layout_.addWidget(&signal_labels_[i], 0, Qt::AlignBottom);
    }

    hor_layout_.addSpacing(10);
    config_button_.setFocusPolicy(Qt::NoFocus);
    hor_layout_.addWidget(&config_button_, 0, Qt::AlignBottom);
    connect(&config_button_, SIGNAL(clicked(bool)), this, SLOT(onConfigButtonClicked()));
}

void WifiAPItem::updateByProfile(WifiProfile & profile)
{
    bool visible = !profile.bssid().isEmpty();
    ssid_label_.setVisible(visible);
    lock_icon_label_.setVisible(visible);
    config_button_.setVisible(visible);
    for(int i = 0; i < SIGNAL_ICONS; ++i)
    {
        signal_labels_[i].setVisible(visible);
    }

    if (profile.bssid().isEmpty())
    {
        update();
        return;
    }

    ssid_label_.setText(profile_.ssid());
    if (profile_.isWep() || profile_.isWpa() || profile_.isWpa2())
    {
        lock_icon_label_.setPixmap(QPixmap(":/images/lock.png"));
    }
    else
    {
        lock_icon_label_.setVisible(false);
    }

    // Boundary check is necessary in case that the quality is out of range.
    int q = profile.quality() * SIGNAL_ICONS / 100;
    if (q * 100 / SIGNAL_ICONS < profile.quality())
    {
        ++q;
    }
    int count = qMin(q, SIGNAL_ICONS);
    for(int i = 0; i < count; ++i)
    {
        QString path(":/images/signal_fg_%1.png");
        path = path.arg(i + 1);
        signal_labels_[i].setPixmap(path);
    }

    for(int i = count; i < SIGNAL_ICONS; ++i)
    {
        QString path(":/images/signal_bk_%1.png");
        path = path.arg(i + 1);
        signal_labels_[i].setPixmap(path);
    }
    update();
}

bool WifiAPItem::isSelected()
{
    return (selected_item_ == this);
}

void WifiAPItem::onConfigButtonClicked()
{
    emit config(profile_);
}

}
