
#include "onyx/sys/dict_conf.h"
#include "onyx/base/device.h"

namespace sys
{

DictConfig::DictConfig()
{
}

DictConfig::~DictConfig()
{
}

bool DictConfig::makeSureTableExist(QSqlDatabase& database)
{
    QSqlQuery query(database);
    bool ok = query.exec("create table if not exists dict_conf ("
                         "key text primary key, "
                         "value text "
                         ")");
    if (ok)
    {
        return query.exec("create index if not exists key_index on dict_conf (key) ");
    }
    return false;
}

/// Return all possible dictionary root directories.
bool DictConfig::dictionaryRoots(QSqlDatabase& db, QStringList & dirs)
{
    dirs.clear();
    QString dir;
#ifndef _WINDOWS
    dir = SHARE_ROOT;
    dir += "/dicts";
    dirs << dir;

    dir = LIBRARY_ROOT;
    dir += "/dicts";
    dirs << dir;

    dir = SDMMC_ROOT;
    dir += "/dicts";
    dirs << dir;

#endif
    dir = QDir::home().absolutePath() + "/dicts";
    dirs << dir;
    return true;
}

QString DictConfig::selectedDictionary(QSqlDatabase& db)
{
    QString ret;
    QSqlQuery query(db);
    query.prepare( "select value from dict_conf where key = ?");
    query.addBindValue("selected_dictionary");
    if (query.exec() && query.next())
    {
        ret = query.value(0).toString();
    }
    return ret;
}

bool DictConfig::selectDictionary(QSqlDatabase& db,
                                  const QString & name)
{
    QSqlQuery query(db);
    query.prepare( "INSERT OR REPLACE into dict_conf (key, value) values(?, ?)");
    query.addBindValue("selected_dictionary");
    query.addBindValue(name);
    return query.exec();
}


}
