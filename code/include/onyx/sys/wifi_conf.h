// Authors: John

/// Public header of the system configuration library.

#ifndef SYS_WIFI_CONF_H__
#define SYS_WIFI_CONF_H__

#include "onyx/base/base.h"
#include "onyx/ui/ui_global.h"
#include <QtSql/QtSql>

namespace sys
{

/// The wifi configurations. It maintains all information that needed
/// to configure wifi interface. It also serves as the parameter that
/// used by wifi manager and caller.
class WifiNetworkProperties : public QMap<QString, QVariant>
{
public:
    WifiNetworkProperties();
    WifiNetworkProperties( const WifiNetworkProperties& list );
    ~WifiNetworkProperties();

public:
    void toByteArray(QByteArray & data);
    void fromByteArray(QByteArray & data);

    // Profile name.
    QString name();
    void setName(const QString &name);

    QString ssid();
    void setSsid(const QString &id);

    // Mac address.
    QByteArray bssid();
    void setBssid(const QByteArray & data);

    QString key_mgmt();
    void setKeyMgmt(const QString &key);

    QString proto();
    void setProto(const QString &proto);

    QString pairwise();
    void setPairwise(const QString &pairwise);

    QString group();
    void setGroup(const QString &group);

    QString psk();
    void setPsk(const QString &psk);

    QString wepKey1();
    void setWepKey1(const QString &key1);

    QString wepKey2();
    void setWepKey2(const QString &key2);

    QString wepKey3();
    void setWepKey3(const QString &key3);

    QString wepKey4();
    void setWepKey4(const QString &key4);

    bool hasAnyWepKey();

    QString authAlg();
    void setAuthAlg(const QString &);

    QString authentication();
    void setAuthentication(const QString &authen);

    int frequency();
    void setFrequency(const int freq);

    int quality();
    void setQuality(const int quality);

    int noise();
    void setNoise(const int noise);

    int level();
    void setLevel(const int level);

    // Max rate in bps.
    int maxrate();
    void setMaxrate(const int freq);

    // capabilities
    unsigned int capabilities();
    void setCapabilities(const unsigned int cap);

    // non-scaned attributes.
    bool isWep();
    void setWep(bool wep);

    bool isWpa();
    void setWpa(bool wpa);

    bool isWpa2();
    void setWpa2(bool wpa2);

    bool isEncrypted();
    void setEncrypted(bool enc);

    int retry();
    void setRetry(const int);
    void increaseRetry();
    void resetRetry();

    // How many times does the connection successfully established.
    int count();
    void setCount(const int);
    void increaseCount();
    void resetCount();

    // ip address
    bool useDHCP();
    void setDHCP(bool dhcp);

    // define by user
    bool isDefinedByUser();
    void defineByUser(bool def);

    void debugDump() const;
};
typedef WifiNetworkProperties WifiProfile;
typedef QVector<WifiProfile> WifiProfiles;

/// Manages all wifi profiles.
class WifiConfig
{
public:
    WifiConfig();
    ~WifiConfig();

private:
    friend class SystemConfig;
    static bool makeSureTableExist(QSqlDatabase& db);
    static bool load(QSqlDatabase& db, WifiProfiles & all);
    static bool save(QSqlDatabase& db, WifiProfiles & all);
    static bool clear(QSqlDatabase& db);
};


void sortByQuality(WifiProfiles & list);
void sortByCount(WifiProfiles & list);

};

#endif  // SYS_WIFI_CONF_H__
