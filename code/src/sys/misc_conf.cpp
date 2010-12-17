
#include "onyx/sys/misc_conf.h"

namespace sys {

static const char * MISC_CONFIG_TABLE = "misc_conf";

MiscConfig::MiscConfig()
{
}

MiscConfig::~MiscConfig()
{
}

bool MiscConfig::makeSureTableExist(QSqlDatabase& database)
{
    QSqlQuery query(database);
    bool ok = query.exec("create table if not exists misc_conf ("
        "key text primary key, "
        "value text "
        ")");
    if (ok)
    {
        return query.exec("create index if not exists key_index on misc_conf (key) ");
    }
    return false;
}

QString MiscConfig::getQueryStatement(const QString prefix,
            const QString suffix)
{
    QString queryStatement(prefix+" ");
    queryStatement += MISC_CONFIG_TABLE;
    queryStatement += " "+suffix;
    return queryStatement;
}

QString MiscConfig::getValue(QSqlDatabase& db, const QString key)
{
    QString ret;
    QSqlQuery query(db);
    QString queryStatement = getQueryStatement("select value from ",
            " where key = ?");
    query.prepare(queryStatement);
    query.addBindValue(key);
    if (query.exec() && query.next())
    {
        ret = query.value(0).toString();
    }
    return ret;
}

bool MiscConfig::setValue(QSqlDatabase& db, const QString key,
        const QString value)
{
    QSqlQuery query(db);
    QString queryStatement = getQueryStatement("INSERT OR REPLACE into ",
            " (key, value) values(?, ?)");
    query.prepare(queryStatement);
    query.addBindValue(key);
    query.addBindValue(value);
    return query.exec();
}

}   // namespace sys

