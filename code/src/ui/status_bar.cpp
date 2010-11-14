#include "onyx/sys/sys.h"
#include "onyx/screen/screen_proxy.h"
#include "onyx/data/network_types.h"

#include "onyx/ui/status_bar.h"
#include "onyx/ui/status_bar_item_menu.h"
#include "onyx/ui/status_bar_item_power.h"
#include "onyx/ui/status_bar_item_slide.h"
#include "onyx/ui/status_bar_item_stylus.h"
#include "onyx/ui/status_bar_item_text.h"
#include "onyx/ui/status_bar_item_clock.h"
#include "onyx/ui/status_bar_item_refresh_screen.h"
#include "onyx/ui/status_bar_item_input_url.h"
#include "onyx/ui/status_bar_item_input.h"
#include "onyx/ui/status_bar_item_connection.h"
#include "onyx/ui/status_bar_item_3g_connection.h"
#include "onyx/ui/status_bar_item_volume.h"
#include "onyx/ui/number_dialog.h"
#include "onyx/ui/power_management_dialog.h"
#include "onyx/ui/clock_dialog.h"
#include "onyx/ui/volume_control.h"

namespace ui
{

static const int HIDE_VOLUME_DIALOG_INTERVAL = 3000;

StatusBar::StatusBar(QWidget *parent, StatusBarItemTypes items)
    : QWidget(parent)
    , items_(0)
    , layout_(this)
    , enable_jump_to_page_(true)
    , hide_volume_dialog_timer_(HIDE_VOLUME_DIALOG_INTERVAL, this, SLOT(onHideVolumeDialog()))
{
    createLayout();
    setupConnections();
    addItems(items);
    initUpdate();
}

StatusBar::~StatusBar(void)
{
}

void StatusBar::setupConnections()
{
    SysStatus & sys_status = SysStatus::instance();
    connect(&sys_status,
            SIGNAL(batterySignal(int, int)),
            this,
            SLOT(onBatterySignal(int, int)));
    connect(&sys_status,
            SIGNAL(aboutToSuspend()),
            this,
            SLOT(onAboutToSuspend()));
    connect(&sys_status,
            SIGNAL(wakeup()),
            this,
            SLOT(onWakeup()));
    connect(&sys_status,
            SIGNAL(aboutToShutdown()),
            this,
            SLOT(onAboutToShutdown()));
    connect(&sys_status,
            SIGNAL(sdioChangedSignal(bool)),
            this,
            SLOT(onWifiDeviceChanged(bool)));
    connect(&sys_status,
            SIGNAL(stylusChanged(bool)),
            this,
            SLOT(onStylusChanged(bool)));
    connect(&sys_status,
            SIGNAL(connectToPC(bool)),
            this,
            SLOT(onConnectToPC(bool)));
    connect(&sys_status,
            SIGNAL(volumeChanged(int, bool)),
            this,
            SLOT(onVolumeChanged(int, bool)));
    connect(&sys_status,
            SIGNAL(report3GNetwork(const int, const int, const int)),
            this,
            SLOT(onReport3GNetwork(const int, const int, const int)));
    connect(&sys_status,
            SIGNAL(pppConnectionChanged(const QString &, int)),
            this,
            SLOT(onPppConnectionChanged(const QString &, int)));
}

/// Update some status when it's created.
void StatusBar::initUpdate()
{
    SysStatus & sys_status = SysStatus::instance();

    int left = 100, status = BATTERY_STATUS_NORMAL;
    sys_status.batteryStatus(left, status);
    changeBatteryStatus(left, status, false);
}

void StatusBar::addItems(StatusBarItemTypes items)
{
    // Adjust the order if necessary.
    const StatusBarItemType all[] =
    {
        MENU, PROGRESS, MESSAGE, STYLUS, CLOCK, INPUT_TEXT, VOLUME, SCREEN_REFRESH, INPUT_URL,THREEG_CONNECTION,
        CONNECTION, BATTERY
    };
    const int size = sizeof(all)/sizeof(all[0]);
    for(int i = 0; i < size; ++i)
    {
        if (items.testFlag(all[i]))
        {
            item(all[i], true);
        }
    }
    items_ = items;
}

int StatusBar::itemState(const StatusBarItemType type)
{
    StatusBarItem *ptr = item(type, false);
    if (ptr)
    {
        return ptr->state();
    }
    return STATE_INVALID;
}

bool StatusBar::setItemState(const StatusBarItemType type,
                             const int state)
{
    return onItemStatusChanged(type, state);
}

bool StatusBar::setMessage(const QString & message)
{
    StatusBarItem *ptr = item(MESSAGE, false);
    if (ptr)
    {
        StatusBarItemMessage *text= static_cast<StatusBarItemMessage*>(ptr);
        text->setMessage(message);
        return true;
    }
    return false;
}

void StatusBar::showItem(StatusBarItemType id, bool show)
{
    StatusBarItem *ptr = item(id, false);
    if (ptr)
    {
        ptr->setVisible(show);
    }
}

bool StatusBar::setProgress(const int value,
                            const int total,
                            bool show_message)
{
    if (total <= 0)
    {
        return false;
    }

    StatusBarItem *ptr = item(PROGRESS, false);
    if (ptr)
    {
        StatusBarItemProgress *wnd = static_cast<StatusBarItemProgress*>(ptr);
        wnd->setProgress(value, total);
    }

    if (show_message && (ptr = item(MESSAGE, false)))
    {
        StatusBarItemMessage *wnd = static_cast<StatusBarItemMessage*>(ptr);
        wnd->setMessage(value, total);
    }
    return true;
}

bool StatusBar::remove(const StatusBarItemType type)
{
    for(StatusBarIter iter = widgets_.begin(); iter != widgets_.end(); ++iter)
    {
        if ((*iter)->type() == type)
        {
            widgets_.erase(iter);
            return true;
        }
    }
    return false;
}

void StatusBar::clear()
{
}

void StatusBar::closeChildrenDialogs()
{
    closeUSBDialog();
}

void StatusBar::closeUSBDialog()
{
    USBConnectionDialog *dialog = usbConnectionDialog(false);
    if (dialog)
    {
        dialog->reject();
        usb_connection_dialog_.reset(0);
    }
}

void StatusBar::mouseMoveEvent(QMouseEvent *me)
{
}

void StatusBar::paintEvent(QPaintEvent *pe)
{
    QPainter painter(this);

    painter.fillRect(rect(), QBrush(QColor(100, 100, 100)));

    QPen pen(QColor(0, 0, 0));
    pen.setWidth(1);

    painter.setPen(pen);
    painter.drawLine(QPoint(0, 0), QPoint(width(), 0));

    pen.setColor(QColor(255, 255, 255));
    painter.setPen(pen);
    painter.drawLine(QPoint(0, 1), QPoint(width(), 1));
}

void StatusBar::onProgressChanging(const int current,
                                   const int total)
{
    onyx::screen::instance().enableUpdate(false);
    StatusBarItem *ptr = item(MESSAGE, false);
    if (ptr)
    {
        StatusBarItemMessage *wnd = static_cast<StatusBarItemMessage*>(ptr);
        wnd->setMessage(current, total);
        wnd->repaint();
    }

    ptr = item(PROGRESS, false);
    if (ptr)
    {
        ptr->repaint();
    }
    QApplication::processEvents();
    onyx::screen::instance().enableUpdate(true);
    onyx::screen::instance().updateWidget(this, onyx::screen::ScreenProxy::GC, false);
}

void StatusBar::onProgressChanged(const int percent,
                                  const int value)
{
    emit progressClicked(percent, value);
}

void StatusBar::onMenuClicked()
{
    emit menuClicked();
}

void StatusBar::onMessageAreaClicked()
{
    if (!isJumpToPageEnabled())
    {
        return;
    }

    int current = 1, total = 1;
    StatusBarItem * wnd = item(PROGRESS, false);
    if (wnd && wnd->isVisible())
    {
        StatusBarItemProgress * p = static_cast<StatusBarItemProgress *>(wnd);
        p->progress(current, total);
    }

    if (total <= 1)
    {
        return;
    }

    // Popup page dialog.
    NumberDialog dialog(0);
    if (dialog.popup(current, total) != QDialog::Accepted)
    {
        onyx::screen::instance().flush(0, onyx::screen::ScreenProxy::GU);
        return;
    }

    current = dialog.value();
    emit progressClicked(current * 100/ total, current);
}

void StatusBar::onBatteryClicked()
{
    PowerManagementDialog dialog(0, sys::SysStatus::instance());
    dialog.exec();
    onyx::screen::instance().flush(0, onyx::screen::ScreenProxy::GU);
}

void StatusBar::onClockClicked()
{
    StatusBarItem *ptr = item(CLOCK, false);
    StatusBarItemClock *clock = 0;
    if (ptr)
    {
        clock = static_cast<StatusBarItemClock*>(ptr);
        clockDialog(true, clock->startDateTime())->exec();
    }
}

void StatusBar::onHideVolumeDialog()
{
    VolumeControlDialog * volume_control_dialog = VolumeControlDialog::instance();
    if (volume_control_dialog->isVisible() && !volume_control_dialog->alwaysActive())
    {
        qDebug("Hide Volume Dialog");
        volume_control_dialog->hide();
        onyx::screen::instance().flush(0, onyx::screen::ScreenProxy::GU);
    }
}

void StatusBar::onVolumeChanged(int new_volume, bool is_mute)
{
    QRegion region = this->visibleRegion();
    if (region.isEmpty())
    {
        qDebug("Hidden Status Bar");
        return;
    }

    QRect visible_rect = region.boundingRect();
    if (visible_rect.width() < height() && visible_rect.height() < height())
    {
        qDebug("Almost Hidden Status Bar");
        return;
    }

    VolumeControlDialog * volume_control_dialog = VolumeControlDialog::instance();
    hide_volume_dialog_timer_.stop();
    if (!volume_control_dialog->isVisible())
    {
        qDebug("Volume Changed, Ensure Volume Dialog Visible");
        volume_control_dialog->ensureVisible();
        onyx::screen::instance().flush(0, onyx::screen::ScreenProxy::GU);
    }
    hide_volume_dialog_timer_.start();
}

void StatusBar::onVolumeClicked()
{
    VolumeControlDialog * volume_control_dialog = VolumeControlDialog::instance();
    hide_volume_dialog_timer_.stop();
    qDebug("Volume Clicked");
    if (!volume_control_dialog->isVisible())
    {
        qDebug("Ensure volume control visible");
        volume_control_dialog->ensureVisible();
        volume_control_dialog->setAlwaysActive(true);
    }
    else
    {
        qDebug("Hide volume control");
        volume_control_dialog->hide();
        volume_control_dialog->setAlwaysActive(false);
    }
    onyx::screen::instance().flush(0, onyx::screen::ScreenProxy::GU);
}

void StatusBar::onScreenRefreshClicked()
{
    onyx::screen::instance().updateScreen(onyx::screen::ScreenProxy::GC);
}

void StatusBar::onInputUrlClicked()
{
    emit requestInputUrl();
}

void StatusBar::onInputTextClicked()
{
    emit requestInputText();
}

/// So far, we still leave this function, actually, it can be
/// merged with setItemState.
bool StatusBar::onItemStatusChanged(const StatusBarItemType type,
                                    const int state)
{
    switch (type)
    {
    case CONNECTION:
        changeConnectionStatus(state);
        return true;
    case STYLUS:
        changeStylus(state);
        return true;
    default:
        break;
    }
    return false;
}

void StatusBar::onBatterySignal(int value, int status)
{
    changeBatteryStatus(value, status, true);
}

void StatusBar::onAboutToSuspend()
{
    qDebug("Status Bar handles about to suspend signal");
}

void StatusBar::onWakeup()
{
    qDebug("Status Bar handles wake up signal");
}

void StatusBar::onAboutToShutdown()
{
    qDebug("Status Bar handles about to shutdown signal");
}

void StatusBar::onWifiDeviceChanged(bool enabled)
{
    qDebug("Status Bar handles wifi device changed signal");
    StatusBarItem *ptr = item(CONNECTION, false);
    if (ptr == 0)
    {
        return;
    }

    if (enabled)
    {
        ptr->show();
    }
    else
    {
        ptr->hide();
    }
}
void StatusBar::onReport3GNetwork(const int signal, const int total, const int network)
{

    StatusBarItem *ptr = item(THREEG_CONNECTION, false);
    if (ptr)
    {
        StatusBarItem3GConnection *wnd = static_cast<StatusBarItem3GConnection*>(ptr);
        onyx::screen::instance().enableUpdate(false);
        bool changed = wnd->signalStrengthChanged(signal, total,network);
        QApplication::processEvents();
        onyx::screen::instance().enableUpdate(true);
        if (changed && isVisible())
        {
            onyx::screen::instance().updateWidget(wnd, onyx::screen::ScreenProxy::GC, false);
        }
    }
}
void StatusBar::onPppConnectionChanged(const QString &message, int value)
{
    if(value == TG_DISCONNECTED)
    {
        onReport3GNetwork(0,5,5);
    }
    else if(value == TG_STOP)
    {
        onReport3GNetwork(-1,5,5);
    }

}


void StatusBar::onStylusChanged(bool inserted)
{
    qDebug("Status Bar handles stylus changed signal");
    StatusBarItem *ptr = item(STYLUS, false);
    if (ptr == 0)
    {
        return;
    }

    onyx::screen::instance().enableUpdate(false);
    if (inserted)
    {
        ptr->show();
    }
    else
    {
        ptr->hide();
    }
    QApplication::processEvents();
    onyx::screen::instance().enableUpdate(true);
    onyx::screen::instance().updateWidget(this, onyx::screen::ScreenProxy::GC);
}

void StatusBar::onConnectToPC(bool connected)
{
    // Popup dialog.
    if (connected && isActiveWindow())
    {
        int ret = usbConnectionDialog(true)->exec();
        closeUSBDialog();
        if (ret != QMessageBox::Yes)
        {
            onyx::screen::instance().updateWidget(0, onyx::screen::ScreenProxy::GU);
            return;
        }
        onyx::screen::instance().flush();
        SysStatus::instance().workInUSBSlaveMode();
    }
    else if (!connected)
    {
        closeUSBDialog();
    }
}
/*
void StatusBar::onUSBCableChanged(bool connected)
{
    // Check if it's visible or not.
    if (!isVisible())
    {
        return;
    }

    // TODO, we need another dialog here to ask if user want to connect
    // to pc or not. After that, we show the semi transparent dialog.
    if (connected)
    {
        semiTransparentDialog(true)->popup();
    }
    else
    {
        transparent_dialog_.reset(0);
    }
}
*/

void StatusBar::changeConnectionStatus(const int conn_status)
{
    StatusBarItem *ptr = item(CONNECTION, false);
    if (ptr)
    {
        StatusBarItemConnection *conn_ptr = static_cast<StatusBarItemConnection*>(ptr);
        conn_ptr->setConnectionStatus(conn_status);
    }
}

void StatusBar::changeBatteryStatus(const int value,
                                    const int status,
                                    bool update_screen)
{
    StatusBarItem *ptr = item(BATTERY, false);
    if (ptr)
    {
        StatusBarItemBattery *wnd = static_cast<StatusBarItemBattery*>(ptr);
        onyx::screen::instance().enableUpdate(false);
        bool changed = wnd->setBatteryStatus(value, status);
        QApplication::processEvents();
        onyx::screen::instance().enableUpdate(true);
        if (update_screen && changed && isActiveWindow())
        {
            onyx::screen::instance().updateWidget(wnd, onyx::screen::ScreenProxy::GC, false);
        }
    }
}

void StatusBar::changeStylus(const int stylus)
{
    StatusBarItem *ptr = item(STYLUS, false);
    if (ptr)
    {
        StatusBarItemStylus *wnd = static_cast<StatusBarItemStylus*>(ptr);
        wnd->setStylusState(stylus);
    }
}

USBConnectionDialog * StatusBar::usbConnectionDialog(bool create)
{
    if (!usb_connection_dialog_ && create)
    {
        usb_connection_dialog_.reset(new USBConnectionDialog(0));
    }
    return usb_connection_dialog_.get();
}

ClockDialog * StatusBar::clockDialog(bool create, const QDateTime & start)
{
    if (!clock_dialog_ && create)
    {
        clock_dialog_.reset(new ClockDialog(start, 0));
    }
    return clock_dialog_.get();
}

void StatusBar::createLayout()
{
    setFixedHeight(35);
    layout_.setSpacing(4);
    layout_.setContentsMargins(1, 2, 1, 0);
}

StatusBarItem *StatusBar::item(const StatusBarItemType type, bool create)
{
    for(StatusBarIter iter = widgets_.begin(); iter != widgets_.end(); ++iter)
    {
        if ((*iter)->type() == type)
        {
            return iter->get();
        }
    }

    if (!create)
    {
        return 0;
    }

    // Construct a new one.
    StatusBarItem *item = 0;
    switch (type)
    {
    case MENU:
        item = new StatusBarItemMenu(this);
        connect(item, SIGNAL(clicked()), this, SLOT(onMenuClicked()));
        break;
    case MESSAGE:
        item = new StatusBarItemMessage(this);
        connect(item, SIGNAL(clicked()), this, SLOT(onMessageAreaClicked()));
        break;
    case BATTERY:
        item = new StatusBarItemBattery(this);
        connect(item, SIGNAL(clicked()), this, SLOT(onBatteryClicked()));
        break;
    case CLOCK:
        item = new StatusBarItemClock(this);
        connect(item, SIGNAL(clicked()), this, SLOT(onClockClicked()));
        break;
    case VOLUME:
        item = new StatusBarItemVolume(this);
        connect(item, SIGNAL(clicked()), this, SLOT(onVolumeClicked()));
        break;
    case SCREEN_REFRESH:
        item = new StatusBarItemRefreshScreen(this);
        connect(item, SIGNAL(clicked()), this, SLOT(onScreenRefreshClicked()));
        break;
    case INPUT_URL:
        item = new StatusBarItemInputUrl(this);
        connect(item, SIGNAL(clicked()), this, SLOT(onInputUrlClicked()));
        break;
    case INPUT_TEXT:
        item = new StatusBarItemInput(this);
        connect(item, SIGNAL(clicked()), this, SLOT(onInputTextClicked()));
        break;
    case CONNECTION:
        item = new StatusBarItemConnection(this);
        break;
    case THREEG_CONNECTION:
        item = new StatusBarItem3GConnection(this);
        break;
    case STYLUS:
        item = new StatusBarItemStylus(this);
        break;
    case PROGRESS:
        item = new StatusBarItemProgress(this);
        connect(item, SIGNAL(changing(const int, const int)),
                this, SLOT(onProgressChanging(const int, const int)));
        connect(item, SIGNAL(clicked(const int, const int)),
                this, SLOT(onProgressChanged(const int, const int)));
        break;
    default:
        break;
    }

    if (item == 0)
    {
        return 0;
    }

    StatusBarItemPtr ptr(item);
    widgets_.push_back(ptr);

    // Place this one into the layout.
    layout_.addWidget(ptr.get());
    return ptr.get();
}

}  // namespace ui
