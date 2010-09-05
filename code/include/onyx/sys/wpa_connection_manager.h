#ifndef WAP_CONNECTION_MANAGER_H_
#define WAP_CONNECTION_MANAGER_H_

#include <string>
#include <QtCore/QtCore>
#include "onyx/base/base.h"
#include "onyx/base/dbus.h"
#include "onyx/sys/wpa_connection.h"
#include "onyx/sys/wifi_conf.h"
#include "onyx/sys/sys_conf.h"
#include "onyx/sys/sys_status.h"

using namespace sys;

/// Wpa Connection Manager to provide for convenience.
/// Message between WpaConnectionManager and WpaConnection
/// - sdio state changed
/// - scan
/// - connection changed
/// - authentication
class WpaConnectionManager : public QObject
{
    Q_OBJECT

public:
    WpaConnectionManager();
    ~WpaConnectionManager();

public Q_SLOTS:
    void enableAutoConnect(bool e) { auto_connect_ = e; }
    bool allowAutoConnect() { return auto_connect_; }

    bool start();
    bool stop();

    void scanResults(WifiProfiles &);

private Q_SLOTS:
    // slots for WpaConnection
    void onSdioChanged(bool on);

    void triggerScan();
    void scan();
    void onScanTimeout();
    void onScanReturned(WifiProfiles & list);

    void onNeedPassword(WifiProfile profile);
    void onConnectionChanged(WifiProfile &, WpaConnection::ConnectionState state);
    void onConnectionTimeout();

    void onComplete();

Q_SIGNALS:
    // signals for caller
    void wpaStateChanged(bool running);
    void connectionChanged(WifiProfile & profile, WpaConnection::ConnectionState state);
    void passwordRequired(WifiProfile profile);

private:
    bool checkWifiDevice();
    bool checkWpaSupplicant();
    void setupConnections();

    void resetProfile(WifiProfile & profile);
    void setPassword(WifiProfile & profile, const QString & password);
    bool checkAuthentication(WifiProfile & profile);
    bool syncAuthentication(WifiProfile & source, WifiProfile & target);
    void saveAp(WifiProfile & profile);

    void increaseScanRetry() { ++scan_count_; }
    void resetScanRetry() { scan_count_ = 0; }
    bool canScanRetry() { return scan_count_ <= 5; }

    bool connectToBestAP();
    bool isConnecting();
    void setConnecting(bool c);
    void stopAllTimers();

    WifiProfiles & records(sys::SystemConfig& conf);

private:
    sys::SysStatus & sys_;
    WpaConnection& proxy_;

    QTimer scan_timer_;
    int scan_count_;     ///< Scan retry.

    WpaConnection::ConnectionState internal_state_;
    bool auto_connect_;
    bool wifi_enabled_;

    WifiProfiles scan_results_;     ///< Also serves as connect list.
    scoped_ptr<WifiProfiles> records_;  ///< All profiles that stored in database.
};

#endif      // WAP_CONNECTION_MANAGER_H_
