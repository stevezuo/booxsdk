
#include "onyx/sys/wifi_conf.h"
#include "onyx/base/device.h"

namespace sys
{

static const QString NAME_TAG = "name";
static const QString SSID_TAG = "ssid";
static const QString BSSID_TAG = "bssid";
static const QString WPAIE_TAG = "wpaie";
static const QString RSNIE_TAG = "rsnie";
static const QString KEYMGMT_TAG = "key_mgmt";
static const QString PROTO_TAG = "proto";
static const QString PAIRWISE_TAG = "pairwise";
static const QString GROUP_TAG = "group";
static const QString PSK_TAG = "psk";
static const QString PASSWORD_TAG = "password";
static const QString WEP_KEY1_TAG = "wep_key1";
static const QString WEP_KEY2_TAG = "wep_key2";
static const QString WEP_KEY3_TAG = "wep_key3";
static const QString WEP_KEY4_TAG = "wep_key4";
static const QString AUTHENTICATION_TAG = "authentication";
static const QString AUTHALG_TAG = "AUTH_ALG";
static const QString FREQUENCY_TAG = "frequency";
static const QString MAXRATE_TAG = "maxrate";
static const QString QUALITY_TAG = "qual";
static const QString NOISE_TAG = "noise";
static const QString LEVEL_TAG = "level";
static const QString CAPABILITIES_TAG = "capabilities";
static const QString WEP_TAG = "wep";
static const QString WEP_TYPE_TAG = "wep_type";
static const QString WPA_TAG = "wpa";
static const QString WPA2_TAG = "wpa2";
static const QString ENCRYPTED_TAG = "encrypted";
static const QString RETRY_TAG = "retry";
static const QString COUNT_TAG = "count";
static const QString DHCP_TAG = "dhcp";
static const QString USER_TAG = "user";

WifiNetworkProperties::WifiNetworkProperties()
    : QMap<QString, QVariant>()
{
}

WifiNetworkProperties::WifiNetworkProperties( const WifiNetworkProperties& list )
    : QMap<QString, QVariant> ( list )
{
}

WifiNetworkProperties::~WifiNetworkProperties()
{
}

void WifiNetworkProperties::toByteArray(QByteArray & data)
{
    QBuffer buffer(&data);
    buffer.open(QIODevice::WriteOnly);
    QDataStream stream(&buffer);
    stream << *this;
}

void WifiNetworkProperties::fromByteArray(QByteArray & data)
{
    QBuffer buffer(&data);
    buffer.open(QIODevice::ReadOnly);
    QDataStream stream(&buffer);
    stream >> *this;
}

QString WifiNetworkProperties::name()
{
    return value(NAME_TAG).toString();
}

void WifiNetworkProperties::setName(const QString &name)
{
    insert(NAME_TAG, name);
}

QString WifiNetworkProperties::ssid()
{
    return value(SSID_TAG).toString();
}

void WifiNetworkProperties::setSsid(const QString &id)
{
    insert(SSID_TAG, id);
}

QByteArray WifiNetworkProperties::bssid()
{
    return value(BSSID_TAG).toByteArray();
}

void WifiNetworkProperties::setBssid(const QByteArray & data)
{
    insert(BSSID_TAG, data);
}

QString WifiNetworkProperties::psk()
{
    return value(PSK_TAG).toString();
}

QString WifiNetworkProperties::key_mgmt()
{
    return value(KEYMGMT_TAG).toString();
}

void WifiNetworkProperties::setKeyMgmt(const QString &key)
{
    insert(KEYMGMT_TAG, key);
}

QString WifiNetworkProperties::proto()
{
    return value(PROTO_TAG).toString();
}

void WifiNetworkProperties::setProto(const QString &proto)
{
    insert(PROTO_TAG, proto);
}

QString WifiNetworkProperties::pairwise()
{
    return value(PAIRWISE_TAG).toString();
}

void WifiNetworkProperties::setPairwise(const QString &pairwise)
{
    insert(PAIRWISE_TAG, pairwise);
}

QString WifiNetworkProperties::group()
{
    return value(GROUP_TAG).toString();
}

void WifiNetworkProperties::setGroup(const QString &group)
{
    insert(GROUP_TAG, group);
}

void WifiNetworkProperties::setPsk(const QString &psk)
{
    insert(PSK_TAG, psk);
}

QString WifiNetworkProperties::wepKey1()
{
    return value(WEP_KEY1_TAG).toString();
}

void WifiNetworkProperties::setWepKey1(const QString &key1)
{
    insert(WEP_KEY1_TAG, key1);
}

QString WifiNetworkProperties::wepKey2()
{
    return value(WEP_KEY2_TAG).toString();
}

void WifiNetworkProperties::setWepKey2(const QString &key2)
{
    insert(WEP_KEY2_TAG, key2);
}

QString WifiNetworkProperties::wepKey3()
{
    return value(WEP_KEY3_TAG).toString();
}

void WifiNetworkProperties::setWepKey3(const QString &key3)
{
    insert(WEP_KEY3_TAG, key3);
}

QString WifiNetworkProperties::wepKey4()
{
    return value(WEP_KEY4_TAG).toString();
}

void WifiNetworkProperties::setWepKey4(const QString &key4)
{
    insert(WEP_KEY4_TAG, key4);
}

bool WifiNetworkProperties::hasAnyWepKey()
{
    return (!wepKey1().isEmpty() || !wepKey2().isEmpty() || !wepKey3().isEmpty() || !wepKey4().isEmpty());
}

QString WifiNetworkProperties::authAlg()
{
    QString result;
    if (contains(AUTHALG_TAG))
    {
        result = value(AUTHALG_TAG).toString();
    }
    return result;
}

/// The type can be: OPEN, SHARED, LEAP
void WifiNetworkProperties::setAuthAlg(const QString &type)
{
    insert(AUTHALG_TAG, type);
}

QString WifiNetworkProperties::authentication()
{
    return value(AUTHENTICATION_TAG).toString();
}

void WifiNetworkProperties::setAuthentication(const QString &authen)
{
    insert(AUTHENTICATION_TAG, authen);
}

int WifiNetworkProperties::frequency()
{
    return value(FREQUENCY_TAG).toInt();
}

void WifiNetworkProperties::setFrequency(const int freq)
{
    insert(FREQUENCY_TAG, freq);
}

int WifiNetworkProperties::quality()
{
    return value(QUALITY_TAG).toInt();
}

void WifiNetworkProperties::setQuality(const int quality)
{
    insert(QUALITY_TAG, quality);
}

int WifiNetworkProperties::noise()
{
    return value(NOISE_TAG).toInt();
}

void WifiNetworkProperties::setNoise(const int noise)
{
    insert(NOISE_TAG, noise);
}

int WifiNetworkProperties::level()
{
    return value(LEVEL_TAG).toInt();
}

void WifiNetworkProperties::setLevel(const int level)
{
    insert(LEVEL_TAG, level);
}

int WifiNetworkProperties::maxrate()
{
    return value(MAXRATE_TAG).toInt();
}

void WifiNetworkProperties::setMaxrate(const int rate)
{
    insert(MAXRATE_TAG, rate);
}

unsigned int WifiNetworkProperties::capabilities()
{
    return value(CAPABILITIES_TAG).toUInt();
}

void WifiNetworkProperties::setCapabilities(const unsigned int cap)
{
    insert(CAPABILITIES_TAG, cap);
}

bool WifiNetworkProperties::isWep()
{
    if (contains(WEP_TAG))
    {
        return value(WEP_TAG).toBool();
    }
    return false;
}

void WifiNetworkProperties::setWep(bool wep)
{
    insert(WEP_TAG, wep);
}

bool WifiNetworkProperties::isWpa()
{
    if (contains(WPA_TAG))
    {
        return value(WPA_TAG).toBool();
    }
    return false;
}

void WifiNetworkProperties::setWpa(bool wpa)
{
    insert(WPA_TAG, wpa);
}

bool WifiNetworkProperties::isWpa2()
{
    if (contains(WPA2_TAG))
    {
        return value(WPA2_TAG).toBool();
    }
    return false;
}

void WifiNetworkProperties::setWpa2(bool wpa2)
{
    insert(WPA2_TAG, wpa2);
}

bool WifiNetworkProperties::isEncrypted()
{
    if (contains(ENCRYPTED_TAG))
    {
        return value(ENCRYPTED_TAG).toBool();
    }
    return false;
}

void WifiNetworkProperties::setEncrypted(bool enc)
{
    insert(ENCRYPTED_TAG, enc);
}

int WifiNetworkProperties::retry()
{
    return value(RETRY_TAG).toInt();
}

void WifiNetworkProperties::setRetry(const int retry)
{
    insert(RETRY_TAG, retry);
}

void WifiNetworkProperties::increaseRetry()
{
    insert(RETRY_TAG, retry() + 1);
}

void WifiNetworkProperties::resetRetry()
{
    insert(RETRY_TAG, 0);
}


int WifiNetworkProperties::count()
{
    return value(COUNT_TAG).toInt();
}

void WifiNetworkProperties::setCount(const int v)
{
    insert(COUNT_TAG, v);
}

void WifiNetworkProperties::increaseCount()
{
    setCount(count() + 1);
}

void WifiNetworkProperties::resetCount()
{
    insert(COUNT_TAG, 0);
}

// By default, we use dhcp.
bool WifiNetworkProperties::useDHCP()
{
    if (contains(DHCP_TAG))
    {
        return value(DHCP_TAG).toBool();
    }
    return true;
}

void WifiNetworkProperties::setDHCP(bool dhcp)
{
    insert(DHCP_TAG, dhcp);
}

bool WifiNetworkProperties::isDefinedByUser()
{
    if (contains(USER_TAG))
    {
        return value(USER_TAG).toBool();
    }
    return false;
}

void WifiNetworkProperties::defineByUser(bool def)
{
    insert(USER_TAG, def);
}

void WifiNetworkProperties::debugDump() const
{
// #ifdef QT_DEBUG
    QString key;
    QString value;

    WifiNetworkProperties::const_iterator i = constBegin();
    qDebug() << "Dumping wifi properties";
    while ( i != constEnd() )
    {
        qDebug() << i.key() << ": " << i.value().toString();
        i++;
    }
// #endif
}


WifiConfig::WifiConfig()
{
}

WifiConfig::~WifiConfig()
{
}

bool WifiConfig::makeSureTableExist(QSqlDatabase& database)
{
    QSqlQuery query(database);
    return query.exec( "create table if not exists wifi_conf ("
                       "key integer primary key, "
                       "value blob)");
}

bool WifiConfig::load(QSqlDatabase& database, WifiProfiles & all)
{
    QSqlQuery query(database);
    query.prepare( "select value from wifi_conf");

    if (!query.exec())
    {
        return false;
    }

    while (query.next())
    {
        WifiProfile profile;
        QByteArray value = query.value(0).toByteArray();
        profile.fromByteArray(value);
        all.push_back(profile);
    }

    return true;
}

bool WifiConfig::save(QSqlDatabase& database, WifiProfiles & all)
{
    clear(database);

    QSqlQuery query(database);
    foreach(WifiProfile profile, all)
    {
        query.prepare( "insert into wifi_conf (value) values(?)");
        QByteArray data;
        profile.toByteArray(data);
        query.addBindValue(data);
        if (!query.exec())
        {
            return false;
        }
    }
    return true;
}

bool WifiConfig::clear(QSqlDatabase& database)
{
    QSqlQuery query(database);
    query.prepare( "delete from wifi_conf");
    return query.exec();
}

/// Define sorting functions.
static bool greaterByQuality( WifiProfile & a, WifiProfile & b )
{
    return (a.quality() > b.quality());
}

static bool greaterByCount( WifiProfile & a, WifiProfile & b )
{
    return (a.count() > b.count());
}


void sortByQuality(WifiProfiles & list)
{
    qSort(list.begin(), list.end(), greaterByQuality);
}

void sortByCount(WifiProfiles & list)
{
    qSort(list.begin(), list.end(), greaterByCount);
}

}
