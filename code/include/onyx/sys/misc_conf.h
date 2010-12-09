
#ifndef SYS_MISC_CONF_H__
#define SYS_MISC_CONF_H__

#include <QtSql/QtSql>

namespace sys
{

class MiscConfig {
public:
    MiscConfig();
    ~MiscConfig();

private:
    friend class SystemConfig;

    static bool makeSureTableExist(QSqlDatabase& db);
    static QString getQueryStatement(const QString prefix,
            const QString suffix);
    static QString getValue(QSqlDatabase& db, const QString key);
    static bool setValue(QSqlDatabase& db, const QString key,
            const QString value);
};

}   // namespace sys

#endif  // SYS_MISC_CONF_H__
