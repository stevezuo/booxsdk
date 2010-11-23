
#include "onyx/wireless/wifi_dialog.h"
#include "onyx/screen/screen_proxy.h"
#include "onyx/wireless/ap_conf_dialog.h"

namespace ui
{
const int SPACING = 2;
const int WIDGET_HEIGHT = 36;
static const int AP_ITEM_HEIGHT = 64;
static const int MARGINS = 10;

static const QString BUTTON_STYLE =    "\
QPushButton                             \
{                                       \
    background: transparent;            \
    font-size: 14px;                    \
    border-width: 1px;                  \
    border-color: transparent;          \
    border-style: solid;                \
    color: black;                       \
    padding: 0px;                       \
}                                       \
QPushButton:pressed                     \
{                                       \
    padding-left: 0px;                  \
    padding-top: 0px;                   \
    border-color: black;                \
    background-color: black;            \
}                                       \
QPushButton:checked                     \
{                                       \
    padding-left: 0px;                  \
    padding-top: 0px;                   \
    color: white;                       \
    border-color: black;                \
    background-color: black;            \
}                                       \
QPushButton:disabled                    \
{                                       \
    padding-left: 0px;                  \
    padding-top: 0px;                   \
    border-color: dark;                 \
    color: dark;                        \
    background-color: white;            \
}";

static onyx::screen::ScreenProxy::Waveform current_wavform = onyx::screen::ScreenProxy::GU;

WifiDialog::WifiDialog(QWidget *parent,
                       SysStatus & sys,
                       const QString& name)
#ifndef Q_WS_QWS
    : QDialog(parent, 0)
#else
    : QDialog(parent, Qt::FramelessWindowHint)
#endif
    , big_box_(this)
    , title_hbox_(0)
    , content_layout_(0)
    , ap_layout_(0)
    , buttons_layout_(0)
    , title_icon_label_(0)
    , title_text_label_(tr("Wireless Connections"), 0)
    , close_button_("", 0)
    , state_widget_(0)
    , prev_button_(QPixmap(":/images/prev_page.png"), "", 0)
    , next_button_(QPixmap(":/images/next_page.png"), "", 0)
    , sys_(sys)
    , proxy_(sys.wpa_proxy(name))
    , count_(0)
    , internal_state_(WpaConnection::STATE_UNKNOWN)
    , auto_connect_to_best_ap_(false)
    , auto_connect_to_default_ap_(true)
    , ap_dialog_visible_(false)
    , wifi_enabled_(false)
{
    setAutoFillBackground(true);
    setBackgroundRole(QPalette::Base);

    createLayout();
    setupConnections();

    scan_timer_.setInterval(1500);
}

WifiDialog::~WifiDialog()
{
}

bool WifiDialog::checkWpaSupplicant()
{
    // Check wpa supplicant is running or not.
    bool need_to_connect = false;
    if (!SysStatus::instance().isWpaSupplicantRunning())
    {
        if (!SysStatus::instance().startWpaSupplicant(""))
        {
            qWarning("Seems we can not start wpa supplicant.");
            return false;
        }
        need_to_connect = true;
    }

    // Always update hardware address
    updateHardwareAddress();

    if (proxy_.openCtrlConnection() >= 0)
    {
        return true;
    }
    return false;
}

bool WifiDialog::checkWifiDevice(bool show_error_dialog)
{
    // Always enable sdio.
    sys_.enableSdio(true);

    // Check state again.
    wifi_enabled_ = sys_.sdioState();
    if (!wifi_enabled_)
    {
        if (show_error_dialog)
        {
            ErrorDialog  dialog(tr("Wifi is closed. Please turn it on."), 0);
            dialog.exec();
            update();
        }
        return false;
    }
    return true;
}

int  WifiDialog::popup(bool scan)
{
    showMaximized();
    scanResults(scan_results_);
    arrangeAPItems(scan_results_);
    onyx::screen::instance().flush(this, onyx::screen::ScreenProxy::GC);
    if (scan)
    {
        triggerScan();
    }
    return exec();
}

void WifiDialog::keyPressEvent(QKeyEvent *)
{
}

void WifiDialog::keyReleaseEvent(QKeyEvent *ke)
{
    if (ke->key() == Qt::Key_Escape)
    {
        ke->accept();
        reject();
    }
}

bool WifiDialog::event(QEvent * e)
{
    bool ret = QDialog::event(e);
    if (e->type() == QEvent::UpdateRequest)
    {
        onyx::screen::instance().updateWidget(this, current_wavform);
        current_wavform = onyx::screen::ScreenProxy::GU;
        e->accept();
        return true;
    }
    return ret;
}

void WifiDialog::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    painter.fillRect(title_hbox_.contentsRect(), QBrush(Qt::black));

    QPainterPath path;
    path.addRoundedRect(content_layout_.contentsRect().adjusted(2, 2, -2, -2), 8, 8, Qt::AbsoluteSize);
    painter.fillPath(path, QBrush(QColor(220, 220, 220)));
    painter.setPen(QColor(Qt::black));
    painter.drawPath(path);
}

void WifiDialog::resizeEvent(QResizeEvent *re)
{
    QDialog::resizeEvent(re);
    paginator_.resize(itemsPerPage());
    arrangeAPItems(scan_results_);
}

void WifiDialog::mousePressEvent(QMouseEvent *)
{
}

void WifiDialog::mouseReleaseEvent(QMouseEvent *)
{
}

void WifiDialog::createLayout()
{
    // big_box_.setSizeConstraint(QLayout::SetMinimumSize);
    big_box_.setSpacing(SPACING);
    big_box_.setContentsMargins(SPACING, SPACING, SPACING, SPACING);

    // title hbox.
    big_box_.addLayout(&title_hbox_);
    title_hbox_.setContentsMargins(0, 0, 0, 0);
    title_hbox_.addWidget(&title_icon_label_, 0, Qt::AlignVCenter);
    title_hbox_.addSpacing(SPACING * 2);
    title_hbox_.addWidget(&title_text_label_, 0, Qt::AlignVCenter);
    title_hbox_.addStretch(0);
    title_hbox_.addWidget(&close_button_);
    title_icon_label_.setPixmap(QPixmap(":/images/network_connection.png"));
    title_text_label_.setAlignment(Qt::AlignVCenter);
    title_icon_label_.setFixedHeight(WIDGET_HEIGHT);
    title_text_label_.useTitleBarStyle();
    title_text_label_.setFixedHeight(WIDGET_HEIGHT);

    close_button_.setStyleSheet(BUTTON_STYLE);
    QPixmap close_pixmap(":/images/close.png");
    close_button_.setIconSize(close_pixmap.size());
    close_button_.setIcon(QIcon(close_pixmap));
    close_button_.setFocusPolicy(Qt::NoFocus);
    QObject::connect(&close_button_, SIGNAL(clicked()), this, SLOT(onCloseClicked()));

    // content layout.
    big_box_.addLayout(&content_layout_);
    content_layout_.setContentsMargins(MARGINS, MARGINS, MARGINS, MARGINS);

    // Status.
    content_layout_.addWidget(&state_widget_);
    QObject::connect(&state_widget_, SIGNAL(refreshClicked()), this, SLOT(onRefreshClicked()));
    QObject::connect(&state_widget_, SIGNAL(customizedClicked()), this, SLOT(onCustomizedClicked()));
    QObject::connect(&prev_button_, SIGNAL(clicked()), this, SLOT(onPrevClicked()), Qt::QueuedConnection);
    QObject::connect(&next_button_, SIGNAL(clicked()), this, SLOT(onNextClicked()), Qt::QueuedConnection);

    // ap layout.
    ap_layout_.setContentsMargins(MARGINS, MARGINS, MARGINS, MARGINS);
    ap_layout_.setSpacing(5);
    content_layout_.addLayout(&ap_layout_);
    content_layout_.addStretch(0);

    // Buttons.
    content_layout_.addLayout(&buttons_layout_);
    buttons_layout_.setContentsMargins(MARGINS, 0, MARGINS, 0);
    prev_button_.setFocusPolicy(Qt::NoFocus);
    next_button_.setFocusPolicy(Qt::NoFocus);
    buttons_layout_.addWidget(&prev_button_);
    buttons_layout_.addStretch(0);
    buttons_layout_.addWidget(&next_button_);
    showPaginationButtons(true, true);

    // Hardware address.
    hardware_address_.setFixedHeight(WIDGET_HEIGHT);
    hardware_address_.setContentsMargins(MARGINS, 0, MARGINS, 0);
    content_layout_.addWidget(&hardware_address_);
}

void WifiDialog::scanResults(WifiProfiles &aps)
{
    proxy_.scanResults(aps);
/*
#ifdef _WINDOWS
    aps.clear();
    for(int i = 0; i < 30; ++i)
    {
        WifiProfile a;
        a.setSsid(QString::number(i));
        QByteArray d("aa:bb:cc:dd:ee:ff");
        a.setBssid(d);
        aps.push_back(a);
    }
    paginator_.reset(0, itemsPerPage(), aps.size());
#endif
    */
}

void WifiDialog::arrangeAPItems(WifiProfiles & profiles)
{
    int count = itemsPerPage();
    for(int i = ap_items_.size(); i < count; ++i)
    {
        WifiAPItem * item = new WifiAPItem(this);
        item->setFixedHeight(AP_ITEM_HEIGHT);
        ap_layout_.addWidget(item);
        ap_items_.push_back(item);
        QObject::connect(item, SIGNAL(clicked(WifiProfile &)), this, SLOT(onAPItemClicked(WifiProfile &)));
        QObject::connect(item, SIGNAL(config(WifiProfile &)), this, SLOT(onAPConfig(WifiProfile &)));
    }

    int bound = qMin(ap_items_.size(), paginator_.offsetInCurrentPage());
    for(int i = 0; i < bound; ++i)
    {
        ap_items_[i]->show();
    }
    for(int i = bound; i < ap_items_.size(); ++i)
    {
        ap_items_[i]->hide();
    }

    bound = qMin(ap_items_.size(), paginator_.offsetInCurrentPage());
    for(int i = 0; i < bound; ++i)
    {
        ap_items_.at(i)->setProfile(profiles[i + paginator_.first_visible()]);
    }
    static WifiProfile dummy;
    for(int i = bound; i < count; ++i)
    {
        ap_items_.at(i)->setProfile(dummy);
    }

    showPaginationButtons(paginator_.isPrevEnable(), paginator_.isNextEnable());
}

int  WifiDialog::itemsPerPage()
{
    int h = big_box_.contentsRect().height();
    h -= title_hbox_.contentsRect().height();
    h -= (content_layout_.margin() * 2 + content_layout_.spacing() * 3);
    h -= (state_widget_.height() + buttons_layout_.contentsRect().height() + hardware_address_.height());
    int items = h / (AP_ITEM_HEIGHT + ap_layout_.spacing());
    if (items <= 0)
    {
        return 3;
    }
    return items;
}

void WifiDialog::setupConnections()
{
    QObject::connect(&scan_timer_, SIGNAL(timeout()), this, SLOT(onScanTimeout()));

    QObject::connect(&proxy_, SIGNAL(scanResultsReady(WifiProfiles &)),
            this, SLOT(onScanReturned(WifiProfiles &)));
    QObject::connect(&proxy_, SIGNAL(stateChanged(WifiProfile &,WpaConnection::ConnectionState)),
        this, SLOT(onConnectionChanged(WifiProfile &, WpaConnection::ConnectionState)));
    QObject::connect(&proxy_, SIGNAL(needPassword(WifiProfile )),
            this, SLOT(onNeedPassword(WifiProfile )));
    QObject::connect(&sys_, SIGNAL(sdioChangedSignal(bool)), this, SLOT(onSdioChanged(bool)));
}

void WifiDialog::clear()
{
}

void WifiDialog::scan()
{
    if (!checkWifiDevice(false))
    {
        updateStateLable(WpaConnection::STATE_DISABLED);
        return;
    }

    increaseRetry();
    bool wpa_ok = checkWpaSupplicant();
    if (wpa_ok && canRetry())
    {
        proxy_.scan();
    }
    else
    {
        scan_timer_.stop();
        if (canRetry())
        {
            scan_timer_.start();
        }
        else if (!wpa_ok)
        {
            // Wifi device is detected, but wpa_supplicant can not be launched
            // Hardware issue, but user can try to turn off and turn on the
            // wifi switcher again.
            updateStateLable(WpaConnection::STATE_HARDWARE_ERROR);
        }
    }
}

void WifiDialog::triggerScan()
{
    sys_.enableSdio(true);
    onSdioChanged(sys_.sdioState());
}

void WifiDialog::connect(const QString & ssid, const QString &password)
{
    proxy_.scanResults(scan_results_);
    foreach(WifiProfile profile, scan_results_)
    {
        if (profile.ssid() == ssid)
        {
            setPassword(profile, password);
            onAPItemClicked(profile);
            return;
        }
    }
}

// So far, disable the auto connecting to best ap.
bool WifiDialog::connectToBestAP()
{
    if (!auto_connect_to_best_ap_)
    {
        return false;
    }
    auto_connect_to_best_ap_ = false;

    sys::SystemConfig conf;
    WifiProfiles & all = records(conf);
    if (all.size() <= 0)
    {
        return false;
    }
    sortByCount(all);
    if (all.front().count() <= 0)
    {
        return false;
    }
    onAPItemClicked(all.front());
    return true;
}

bool WifiDialog::connectToDefaultAP()
{
    if (!auto_connect_to_default_ap_)
    {
        return false;
    }
    auto_connect_to_default_ap_ = false;

    QString ap = sys::SystemConfig::defaultAccessPoint();
    if (!ap.isEmpty())
    {
        for(int i = 0; i < scan_results_.size(); ++i)
        {
            if (scan_results_[i].ssid().contains(ap, Qt::CaseInsensitive))
            {
                onAPItemClicked(scan_results_[i]);
                return true;
            }
        }
    } 

    return false;
}

void WifiDialog::onScanTimeout()
{
    // If we can not connect to wpa supplicant before, we need to
    // scan now.
    scan();
}

void WifiDialog::onConnectionTimeout()
{
    // Need to clean up password.
    // Timeout in wifi dialog.
    updateStateLable(WpaConnection::STATE_TIMEOUT);
}

void WifiDialog::onAPItemClicked(WifiProfile & profile)
{
    //if (proxy_.isWpaSupplicantConnected())
    {
        setConnecting(true);
        proxy_.connectTo(profile);
    }
    update();
}

bool WifiDialog::isConnecting()
{
    return (internal_state_ == WpaConnection::STATE_CONNECTING);
}

void WifiDialog::setConnecting(bool c)
{
    if (c)
    {
        internal_state_ = WpaConnection::STATE_CONNECTING;
    }
    else
    {
        internal_state_ = WpaConnection::STATE_UNKNOWN;
    }
}

void WifiDialog::stopAllTimers()
{
    scan_timer_.stop();
}

/// Refresh handler. We try to scan if wpa_supplicant has been launched.
/// Otherwise, we need to make sure wpa_supplicant is launched and it
/// acquired the system bus.
void WifiDialog::onRefreshClicked()
{
    setConnecting(false);
    resetRetry();
    scan();
}

void WifiDialog::onPrevClicked()
{
    if (paginator_.prev())
    {
        current_wavform = onyx::screen::ScreenProxy::GC;
        arrangeAPItems(scan_results_);
    }
}

void WifiDialog::onNextClicked()
{
    if (paginator_.next())
    {
        current_wavform = onyx::screen::ScreenProxy::GC;
        arrangeAPItems(scan_results_);
    }
}

void WifiDialog::onCustomizedClicked()
{
    sys::WifiProfile profile;
    if (showConfigurationDialog(profile))
    {
        onAPItemClicked(profile);
    }

}

void WifiDialog::onCloseClicked()
{
    reject();
}

void WifiDialog::onSdioChanged(bool on)
{
    wifi_enabled_ = on;
    if (on)
    {
        onRefreshClicked();
        enableChildren(on);
        updateHardwareAddress();
    }
    else
    {
        updateStateLable(WpaConnection::STATE_DISABLED);
        enableChildren(on);
    }
}

void WifiDialog::enableChildren(bool enable)
{
    state_widget_.setEnabled(enable);
    foreach(WifiAPItem * p, ap_items_)
    {
        p->setEnabled(enable);
    }
}

void WifiDialog::onScanReturned(WifiProfiles & list)
{
    // When connecting, we may igonre the list.
    // TODO, we can also merge them into together.
    if (isConnecting())
    {
        qDebug("Don't trigger scan as it's in connecting state.");
        return;
    }

    paginator_.reset(0, itemsPerPage(), list.size());
    scan_results_ = list;

    // Retry to use profiles that have been used.
    sys::SystemConfig conf;
    WifiProfiles & all = records(conf);
    conf.close();

    foreach(WifiProfile record, all)
    {
        for(int i = 0; i < scan_results_.size(); ++i)
        {
            if (scan_results_[i].bssid() == record.bssid())
            {
                syncAuthentication(record, scan_results_[i]);
            }
        }
    }

    arrangeAPItems(scan_results_);

    // If there is no scan result, we retry.
    if (scan_results_.size() <= 0)
    {
        scan();
        return;
    }

    scan_timer_.stop();
    onyx::screen::instance().flush();
    onyx::screen::instance().updateWidget(this, onyx::screen::ScreenProxy::GC);

    if (!connectToDefaultAP())
    {
        // Disable auto connect.
        connectToBestAP();
    }
}

void WifiDialog::onConnectionChanged(WifiProfile & profile, WpaConnection::ConnectionState state)
{
    updateStateLable(state);
    if (state == WpaConnection::STATE_CONNECTED)
    {
        storeAp(profile);
    }
    if (state == WpaConnection::STATE_COMPLETE)
    {
        stopAllTimers();
    }
    else if (state == WpaConnection::STATE_ACQUIRING_ADDRESS_ERROR)
    {
        resetProfile(profile);
    }
    else if (state == WpaConnection::STATE_AUTHENTICATION_FAILED)
    {
        // Could be ignored now.
        if (!checkAuthentication(profile))
        {
            onNeedPassword(profile);
        }
    }
    //emit connectionChanged(profile, state);
}

void WifiDialog::updateStateLable(WpaConnection::ConnectionState state)
{
    // If wifi is disabled.
    if (!wifi_enabled_ && state != WpaConnection::STATE_DISABLED)
    {
        return;
    }

    // qDebug("WifiDialog::updateStateLable %d", state);
    switch (state)
    {
    case WpaConnection::STATE_DISABLED:
        state_widget_.setState(tr("Wifi is disabled."));
        break;
    case WpaConnection::STATE_HARDWARE_ERROR:
        state_widget_.setState(tr("Can not start wifi device."));
        break;
    case WpaConnection::STATE_SCANNING:
        state_widget_.setState(tr("Scanning..."));
        break;
    case WpaConnection::STATE_SCANNED:
        if (!isConnecting())
        {
            state_widget_.setState(tr("Ready"));
        }
        break;
    case WpaConnection::STATE_CONNECTING:
        state_widget_.setState(tr("Connecting..."));
        break;
    case WpaConnection::STATE_CONNECTED:
    case WpaConnection::STATE_ACQUIRING_ADDRESS:
        state_widget_.setState(tr("Connected. Acquiring address..."));
        break;
    case WpaConnection::STATE_ACQUIRING_ADDRESS_ERROR:
        state_widget_.setState(tr("Could not acquire address."));
        break;
    case WpaConnection::STATE_COMPLETE:
        {
            QString text(tr("Connected: %1"));
            text = text.arg(proxy_.address());
            state_widget_.setState(text);
            QTimer::singleShot(0, this, SLOT(onComplete()));
        }
        break;
    case WpaConnection::STATE_CONNECT_ERROR:
    case WpaConnection::STATE_DISCONNECT:
        state_widget_.setState(tr("Not connected."));
        break;
    case WpaConnection::STATE_TIMEOUT:
        state_widget_.setState(tr("Timeout."));
        break;
    case WpaConnection::STATE_ABORTED:
        state_widget_.setState(tr("Aborted."));
    default:
        break;
    }
}

/// Authentication handler. When the AP needs psk, we try to check
/// if the password is stored in database or not. If possible, we
/// can use the password remembered.
void WifiDialog::onNeedPassword(WifiProfile profile)
{
    if (ap_dialog_visible_)
    {
        return;
    }
    qDebug("need password now");

    // If password is remembered and correct, we use it directly.
    if (checkAuthentication(profile))
    {
        proxy_.connectTo(profile);
        return;
    }

    // No password remembered or incorrect.
    bool ok = showConfigurationDialog(profile);
    if (ok)
    {
        // We can store the AP here as user already updated password.
        storeAp(profile);

        // Connect again.
        proxy_.connectTo(profile);
    }
    else
    {
        proxy_.stop();
        updateStateLable(WpaConnection::STATE_ABORTED);
    }
}

void WifiDialog::onComplete()
{
    accept();
}

void WifiDialog::onAPConfig(WifiProfile &profile)
{
    if (showConfigurationDialog(profile))
    {
        onAPItemClicked(profile);
    }
}

void WifiDialog::resetProfile(WifiProfile & profile)
{
    setPassword(profile, "");
    profile.setCount(-1);
    storeAp(profile);
}

void WifiDialog::setPassword(WifiProfile & profile,
                             const QString & password)
{
    if (profile.isWpa() || profile.isWpa2())
    {
        profile.setPsk(password);
    }
    else if (profile.isWep())
    {
        profile.setWepKey1(password);
    }
}


/// Check the authentication data for the specified access point.
bool WifiDialog::checkAuthentication(WifiProfile & profile)
{
    // Retry to use profiles that have been used.
    sys::SystemConfig conf;
    WifiProfiles & all = records(conf);
    conf.close();

    foreach(WifiProfile record, all)
    {
        // Use bssid instead of ssid.
        if (profile.bssid() == record.bssid())
        {
            if (profile.retry() <= 2)
            {
                if (syncAuthentication(record, profile))
                {
                    profile.setCount(record.count());
                    profile.setRetry(profile.retry() + 1);
                    return (profile.retry() <= 2);
                }
            }
        }
    }
    return false;
}

bool WifiDialog::syncAuthentication(WifiProfile & source,
                                    WifiProfile & target)
{
    if (source.isWep() && target.isWep())
    {
        target.setWepKey1(source.wepKey1());
        target.setAuthAlg(source.authAlg());
        return true;
    }

    if ((source.isWpa() && target.isWpa()) ||
        (source.isWpa2() && target.isWpa2()))
    {
        if (source.psk().isEmpty())
        {
            return false;
        }
        target.setPsk(source.psk());
        return true;
    }
    return false;
}

/// Store access point that successfully connected.
void WifiDialog::storeAp(WifiProfile & profile)
{
    sys::SystemConfig conf;
    WifiProfiles & all = records(conf);

    profile.increaseCount();
    for(int i = 0; i < records_->size(); ++i)
    {
        if (all[i].bssid() == profile.bssid())
        {
            profile.resetRetry();
            all.replace(i, profile);
            conf.saveWifiProfiles(all);
            return;
        }
    }

    all.push_front(profile);
    conf.saveWifiProfiles(all);
}

WifiProfiles & WifiDialog::records(sys::SystemConfig &conf)
{
    if (!records_)
    {
        records_.reset(new WifiProfiles);
        conf.loadWifiProfiles(*records_);
    }
    return *records_;
}

void WifiDialog::updateHardwareAddress()
{
    QString text(tr("Hardware Address: %1"));
    text = text.arg(proxy_.hardwareAddress());
    hardware_address_.setText(text);
}

bool WifiDialog::showConfigurationDialog(WifiProfile &profile)
{
    ap_dialog_visible_ = true;
    ApConfigDialog dialog(this, profile);
    int ret = dialog.popup();
    ap_dialog_visible_ = false;

    // Update screen.
    onyx::screen::instance().flush(500);
    onyx::screen::instance().updateWidget(this, onyx::screen::ScreenProxy::GC);

    // Check return value.
    if (ret == QDialog::Accepted)
    {
        return true;
    }
    return false;
}

void WifiDialog::showPaginationButtons(bool show_prev,
                                          bool show_next)
{
    prev_button_.setVisible(show_prev);
    next_button_.setVisible(show_next);
}

}   // namespace ui

///
/// \example wifi/wifi_gui_test.cpp
/// This is an example of how to use the WifiDialog class.
///
