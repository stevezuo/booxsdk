
#include "onyx/sys/dialup_conf.h"
#include "onyx/base/device.h"

namespace sys
{

static const QString NAME_TAG = "name";
static const QString NUMBER_TAG = "number";
static const QString USERNAME_TAG = "username";
static const QString PASSWORD_TAG = "password";
static const QString RETRY_TAG = "retry";
static const QString COUNT_TAG = "count";


DialupProperties::DialupProperties()
    : QMap<QString, QVariant>()
{
}

DialupProperties::DialupProperties( const DialupProperties& list )
    : QMap<QString, QVariant> ( list )
{
}

DialupProperties::~DialupProperties()
{
}

void DialupProperties::toByteArray(QByteArray & data)
{
    QBuffer buffer(&data);
    buffer.open(QIODevice::WriteOnly);
    QDataStream stream(&buffer);
    stream << *this;
}

void DialupProperties::fromByteArray(QByteArray & data)
{
    QBuffer buffer(&data);
    buffer.open(QIODevice::ReadOnly);
    QDataStream stream(&buffer);
    stream >> *this;
}

QString DialupProperties::name()
{
    return value(NAME_TAG).toString();
}

void DialupProperties::setName(const QString &name)
{
    insert(NAME_TAG, name);
}

QString DialupProperties::number()
{
    return value(NUMBER_TAG).toString();
}

void DialupProperties::setNumber(const QString &number)
{
    insert(NUMBER_TAG, number);
}

QString DialupProperties::username()
{
    return value(USERNAME_TAG).toString();
}

void DialupProperties::setUsername(const QString &username)
{
    insert(USERNAME_TAG, username);
}

QString DialupProperties::password()
{
    return value(PASSWORD_TAG).toString();
}

void DialupProperties::setPassword(const QString & password)
{
    insert(PASSWORD_TAG, password);
}

QString DialupProperties::defaultPeer()
{
    return qgetenv("DEFAULT_PEER");
}

void DialupProperties::debugDump() const
{
// #ifdef QT_DEBUG
    QString key;
    QString value;

    DialupProperties::const_iterator i = constBegin();
    qDebug() << "Dumping Dialup properties";
    while ( i != constEnd() )
    {
        qDebug() << i.key() << ": " << i.value().toString();
        i++;
    }
// #endif
}


DialupConfig::DialupConfig()
{
}

DialupConfig::~DialupConfig()
{
}

bool DialupConfig::makeSureTableExist(QSqlDatabase& database)
{
    QSqlQuery query(database);
    return query.exec( "create table if not exists Dialup_conf ("
                       "key integer primary key, "
                       "value blob)");
}

bool DialupConfig::load(QSqlDatabase& database, DialupProfiles & all)
{
    QSqlQuery query(database);
    query.prepare( "select value from Dialup_conf");

    if (!query.exec())
    {
        return false;
    }

    while (query.next())
    {
        DialupProfile profile;
        QByteArray value = query.value(0).toByteArray();
        profile.fromByteArray(value);
        all.push_back(profile);
    }

    return true;
}

bool DialupConfig::save(QSqlDatabase& database, DialupProfiles & all)
{
    clear(database);

    QSqlQuery query(database);
    foreach(DialupProfile profile, all)
    {
        query.prepare( "insert into Dialup_conf (value) values(?)");
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

bool DialupConfig::clear(QSqlDatabase& database)
{
    QSqlQuery query(database);
    query.prepare( "delete from Dialup_conf");
    return query.exec();
}

}
