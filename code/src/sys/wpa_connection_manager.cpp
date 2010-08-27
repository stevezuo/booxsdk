
#include "onyx/sys/wpa_connection_manager.h"


WpaConnectionManager::WpaConnectionManager()
: sys_(sys::SysStatus::instance())
, proxy_(sys::SysStatus::instance().wpa_proxy())
, scan_count_(0)
, internal_state_(WpaConnection::STATE_UNKNOWN)
, auto_connect_(true)
, wifi_enabled_(true)
{
    setupConnections();
    scan_timer_.setInterval(1500);
}

WpaConnectionManager::~WpaConnectionManager()
{
}

// Reset state machine and start the following actions
// - scan
// - connect to certain if ap is connected before
// - automatically fill password if necessary
// - broadcast signals when state changed
bool WpaConnectionManager::start()
{
    triggerScan();
    return true;
}

bool WpaConnectionManager::stop()
{
    sys_.enableSdio(false);
    return true;
}

void WpaConnectionManager::onSdioChanged(bool on)
{
    if (on)
    {
        start();
    }
    else
    {
        stop();
    }
}

void WpaConnectionManager::onScanTimeout()
{
    // If we can not connect to wpa supplicant before, we need to
    // scan now.
    scan();
}

/// Called when wpa supplicant get some scan results.
void WpaConnectionManager::onScanReturned(WifiProfiles & list)
{
    // When connecting, we may igonre the list.
    // TODO, we can also merge them into together.
    if (isConnecting())
    {
        qDebug("Don't trigger scan as it's in connecting state.");
        return;
    }

    // If there is no scan result, we retry.
    if (list.size() <= 0)
    {
        scan();
        return;
    }

    scan_results_ = list;
    scan_timer_.stop();
    connectToBestAP();
}

void WpaConnectionManager::scanResults(WifiProfiles &ret)
{
    ret = scan_results_;
}

void WpaConnectionManager::onNeedPassword(WifiProfile profile)
{
    // password is incorrect
}

void WpaConnectionManager::onConnectionChanged(WifiProfile &profile,
                                               WpaConnection::ConnectionState state)
{
    if (state == WpaConnection::STATE_CONNECTED)
    {
        saveAp(profile);
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
}

void WpaConnectionManager::onConnectionTimeout()
{
}

void WpaConnectionManager::onComplete()
{
}

bool WpaConnectionManager::checkWifiDevice()
{
    // Always enable sdio.
    sys_.enableSdio(true);

    // Check state again.
    wifi_enabled_ = sys_.sdioState();
    if (!wifi_enabled_)
    {
        return false;
    }
    return true;
}

bool WpaConnectionManager::checkWpaSupplicant()
{
    // Check wpa supplicant is running or not.
    if (!SysStatus::instance().isWpaSupplicantRunning())
    {
        if (!SysStatus::instance().startWpaSupplicant(""))
        {
            qWarning("Seems we can not start wpa supplicant.");
            emit wpaStateChanged(false);
            return false;
        }
    }

    if (proxy_.openCtrlConnection() >= 0)
    {
        emit wpaStateChanged(true);
        return true;
    }
    emit wpaStateChanged(false);
    return false;
}

void WpaConnectionManager::setupConnections()
{
    QObject::connect(&scan_timer_, SIGNAL(timeout()), this, SLOT(onScanTimeout()));

    SysStatus & sys = sys::SysStatus::instance();
    QObject::connect(&sys, SIGNAL(sdioChangedSignal(bool)), this, SLOT(onSdioChanged(bool)));

    QObject::connect(&proxy_, SIGNAL(scanResultsReady(WifiProfiles &)),
            this, SLOT(onScanReturned(WifiProfiles &)));
    QObject::connect(&proxy_, SIGNAL(stateChanged(WifiProfile &,WpaConnection::ConnectionState)),
        this, SLOT(onConnectionChanged(WifiProfile &, WpaConnection::ConnectionState)));
    QObject::connect(&proxy_, SIGNAL(needPassword(WifiProfile )),
            this, SLOT(onNeedPassword(WifiProfile )));

}

void WpaConnectionManager::triggerScan()
{
    setConnecting(false);
    resetScanRetry();
    scan();
}

void WpaConnectionManager::scan()
{
    if (!checkWifiDevice())
    {
        return;
    }

    increaseScanRetry();
    bool wpa_ok = checkWpaSupplicant();
    if (wpa_ok && canScanRetry())
    {
        proxy_.scan();
    }
    else
    {
        scan_timer_.stop();
        if (canScanRetry())
        {
            scan_timer_.start();
        }
        else if (!wpa_ok)
        {
            // Wifi device is detected, but wpa_supplicant can not be launched
            // Hardware issue, but user can try to turn off and turn on the
            // wifi switcher again.

        }
    }
}

void WpaConnectionManager::resetProfile(WifiProfile & profile)
{
    // TODO, not sure.
    setPassword(profile, QString());
    profile.setCount(-1);
    saveAp(profile);
}

void WpaConnectionManager::setPassword(WifiProfile & profile, const QString & password)
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

bool WpaConnectionManager::checkAuthentication(WifiProfile & profile)
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

bool WpaConnectionManager::syncAuthentication(WifiProfile & source,
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

void WpaConnectionManager::saveAp(WifiProfile & profile)
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

bool WpaConnectionManager::connectToBestAP()
{
    if (!allowAutoConnect())
    {
        return false;
    }

    // Steps:
    // - Load all records and sort them according to count.
    // - Sort scan results according to the signal strength.
    // - Check if it's connected before, if ok, connect.
    sortByQuality(scan_results_);

    sys::SystemConfig conf;
    WifiProfiles & all = records(conf);
    conf.close();
    sortByCount(all);

    foreach(WifiProfile record, all)
    {
        for(int i = 0; i < scan_results_.size(); ++i)
        {
            if (scan_results_[i].bssid() == record.bssid())
            {
                syncAuthentication(record, scan_results_[i]);
                scan_results_[i].setCount(record.count());
            }
        }
    }

    sortByCount(scan_results_);

    setConnecting(true);
    proxy_.connectTo(scan_results_.front());

    return true;
}

bool WpaConnectionManager::isConnecting()
{
    return (internal_state_ == WpaConnection::STATE_CONNECTING);
}

void WpaConnectionManager::setConnecting(bool c)
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

void WpaConnectionManager::stopAllTimers()
{
    scan_timer_.stop();
}

WifiProfiles & WpaConnectionManager::records(sys::SystemConfig& conf)
{
    if (!records_)
    {
        records_.reset(new WifiProfiles);
        conf.loadWifiProfiles(*records_);
    }
    return *records_;
}
