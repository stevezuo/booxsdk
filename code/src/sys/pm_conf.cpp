
#include "onyx/base/base.h"
#include "onyx/sys/pm_conf.h"


namespace sys
{


PMConfig::PMConfig()
{
}

PMConfig::~PMConfig()
{
}

bool PMConfig::makeSureTableExist(QSqlDatabase &database)
{
    QSqlQuery query(database);
    bool ok = query.exec("create table if not exists power_management ("
                         "key text primary key, "
                         "value text "
                         ")");
    if (ok)
    {
        return query.exec("create index if not exists key_index on power_management (key) ");
    }
    return false;
}

/// Return the suspend interval in ms.
int PMConfig::suspendInterval(QSqlDatabase &database)
{
    QSqlQuery query(database);
    query.prepare( "select value from power_management where key = ? ");
    query.addBindValue("suspend_interval");
    int interval = 300 * 1000;
    int t = qgetenv("SUSPEND_INTERVAL").toInt();
    if (t > 0)
    {
        interval = t;
    }
    if (query.exec() && query.next())
    {
        interval = query.value(0).toInt();
    }
    return interval;
}

bool PMConfig::setSuspendInterval(QSqlDatabase &database, int ms)
{
    QSqlQuery query(database);
    query.prepare( "INSERT OR REPLACE into power_management (key, value) values(?, ?)");
    query.addBindValue("suspend_interval");
    query.addBindValue(ms);
    return query.exec();
}

int PMConfig::shutdownInterval(QSqlDatabase &database)
{
    QSqlQuery query(database);
    query.prepare( "select value from power_management where key = ? ");
    query.addBindValue("shutdown_interval");
    int interval = 0;
    int t = qgetenv("SHUTDOWN_INTERVAL").toInt();
    if (t > 0)
    {
        interval = t;
    }
    if (query.exec() && query.next())
    {
        interval = query.value(0).toInt();
    }
    return interval;
}

bool PMConfig::setShutdownInterval(QSqlDatabase &database, int seconds)
{
    QSqlQuery query(database);
    query.prepare( "INSERT OR REPLACE into power_management (key, value) values(?, ?)");
    query.addBindValue("shutdown_interval");
    query.addBindValue(seconds);
    return query.exec();
}

}
