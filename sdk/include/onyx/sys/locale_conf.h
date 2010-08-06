// Authors: John

/// Public header of the system configuration library.

#ifndef SYS_LOCALE_CONF_H__
#define SYS_LOCALE_CONF_H__

#include <QtSql/QtSql>

namespace sys
{

class LocaleConfig
{
public:
    LocaleConfig();
    ~LocaleConfig();

private:
    friend class SystemConfig;

    static bool makeSureTableExist(QSqlDatabase& db);
    static QLocale locale(QSqlDatabase& db);
    static bool setLocale(QSqlDatabase & db,
                          const QLocale &locale);
};

};

#endif  // SYS_LOCALE_CONF_H__
