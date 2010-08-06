
#include "onyx/base/base.h"
#include "onyx/sys/volume_conf.h"


namespace sys
{


VolumeConfig::VolumeConfig()
{
}

VolumeConfig::~VolumeConfig()
{
}

bool VolumeConfig::makeSureTableExist(QSqlDatabase &database)
{
    QSqlQuery query(database);
    bool ok = query.exec("create table if not exists volume ("
                         "key text primary key, "
                         "value text "
                         ")");
    if (ok)
    {
        return query.exec("create index if not exists key_index on volume (key) ");
    }
    return false;
}

int VolumeConfig::volume(QSqlDatabase &database)
{
    QSqlQuery query(database);
    query.prepare( "select value from volume where key = ? ");
    query.addBindValue("volume");
    int volume = -1;
    if (query.exec() && query.next())
    {
        volume = query.value(0).toInt();
    }
    return volume;
}

bool VolumeConfig::setVolume(QSqlDatabase &database, int volume)
{
    QSqlQuery query(database);
    query.prepare( "INSERT OR REPLACE into volume (key, value) values(?, ?)");
    query.addBindValue("volume");
    query.addBindValue(volume);
    return query.exec();
}

bool VolumeConfig::mute(QSqlDatabase &database, bool m)
{
    QSqlQuery query(database);
    query.prepare( "INSERT OR REPLACE into volume (key, value) values(?, ?)");
    query.addBindValue("mute");
    query.addBindValue(m);
    return query.exec();
}

bool VolumeConfig::isMute(QSqlDatabase &database)
{
    QSqlQuery query(database);
    query.prepare( "select value from volume where key = ? ");
    query.addBindValue("mute");
    bool mute = false;
    if (query.exec() && query.next())
    {
        mute = query.value(0).toBool();
    }
    return mute;
}

}
