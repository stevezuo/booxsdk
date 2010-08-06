// Authors: John

/// Public header of the system configuration library.

#ifndef SYS_DIALUP_CONF_H__
#define SYS_DIALUP_CONF_H__

#include "onyx/base/base.h"
#include "onyx/ui/ui_global.h"
#include <QtSql/QtSql>

namespace sys
{

/// The Dialup configurations. It maintains all information that needed
/// to configure Dialup network.
class DialupProperties : public QMap<QString, QVariant>
{
public:
    DialupProperties();
    DialupProperties( const DialupProperties& list );
    ~DialupProperties();

public:
    void toByteArray(QByteArray & data);
    void fromByteArray(QByteArray & data);

    // Profile name.
    QString name();
    void setName(const QString &name);

    QString number();
    void setNumber(const QString &number);

    // user name.
    QString username();
    void setUsername(const QString &name);

    // password
    QString password();
    void setPassword(const QString &password);

    void debugDump() const;
};
typedef DialupProperties DialupProfile;
typedef QVector<DialupProfile> DialupProfiles;

/// Manages all Dialup profiles.
class DialupConfig
{
public:
    DialupConfig();
    ~DialupConfig();

private:
    friend class SystemConfig;
    static bool makeSureTableExist(QSqlDatabase& db);
    static bool load(QSqlDatabase& db, DialupProfiles & all);
    static bool save(QSqlDatabase& db, DialupProfiles & all);
    static bool clear(QSqlDatabase& db);
};


};

#endif  // SYS_Dialup_CONF_H__
