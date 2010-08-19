// Author: John

/// This file defines StatusBar. It should be used by other
/// packages in the code base to use statusbar.

#ifndef ONYX_STATUS_BAR_H_
#define ONYX_STATUS_BAR_H_

#include "onyx/base/base.h"
#include "onyx/ui/ui_global.h"
#include "status_bar_item.h"
#include "common_dialogs.h"

namespace ui
{

/// Status bar library for GUI shell and other applications.
/// Status bar separates the whole bar into three parts:
/// - Menu area.
/// - Message area.
/// - System area.
class StatusBar : public QWidget
{
    Q_OBJECT
public:
    StatusBar(QWidget *parent, StatusBarItemTypes items = MENU|PROGRESS|MESSAGE|BATTERY);
    ~StatusBar(void);

public:
    void addItems(StatusBarItemTypes ids);
    int itemState(const StatusBarItemType type);

public Q_SLOTS:
    void showItem(StatusBarItemType item, bool show = true);
    bool setProgress(const int value, const int total, bool show_message = true);
    bool setMessage(const QString & message);
    bool setItemState(const StatusBarItemType type, const int state);

    void enableJumpToPage(bool enable) { enable_jump_to_page_ = enable; }
    bool isJumpToPageEnabled() { return enable_jump_to_page_; }

    bool remove(const StatusBarItemType type);
    void clear();
    void closeChildrenDialogs();
    void closeUSBDialog();
    void onMessageAreaClicked();
    void onBatteryClicked();
    void onClockClicked();
    void onScreenRefreshClicked();
    void onInputUrlClicked();
    void onInputTextClicked();
    void onVolumeClicked();

Q_SIGNALS:
    void progressClicked(const int percent, const int value);
    void menuClicked();
    void requestInputUrl();
    void requestInputText();

private Q_SLOTS:
    void onProgressChanging(const int current, const int total);
    void onProgressChanged(const int percent, const int value);
    void onMenuClicked();
    bool onItemStatusChanged(const StatusBarItemType type, const int state);

    // handle the events from system status manager
    void onBatterySignal(int value, int status);
    void onAboutToSuspend();
    void onWakeup();
    void onAboutToShutdown();
    void onWifiDeviceChanged(bool enabled);
    void onStylusChanged(bool inserted);
    void onConnectToPC(bool);
    void onVolumeChanged(int new_volume, bool is_mute);
    void onHideVolumeDialog();

private:
    virtual void mouseMoveEvent(QMouseEvent *me);
    virtual void paintEvent(QPaintEvent *pe);

private:
    void createLayout();
    void setupConnections();
    void initUpdate();
    StatusBarItem *item(const StatusBarItemType type, bool create = true);

    void changeConnectionStatus(const int conn_status);
    void changeBatteryStatus(const int value, const int status, bool update_screen);
    void changeStylus(const int stylus);

    USBConnectionDialog * usbConnectionDialog(bool create);

private:
    typedef shared_ptr<StatusBarItem>          StatusBarItemPtr;
    typedef vector<StatusBarItemPtr>           StatusBarItems;
    typedef vector<StatusBarItemPtr>::iterator StatusBarIter;

private:
    StatusBarItemTypes items_;
    QHBoxLayout        layout_;
    StatusBarItems     widgets_;
    bool               enable_jump_to_page_;
    scoped_ptr<USBConnectionDialog> usb_connection_dialog_;
    QTimer             hide_volume_dialog_timer_;
};

};  // namespace ui

#endif  // ONYX_STATUS_BAR_H_
