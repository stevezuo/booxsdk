// Author: John

#include "onyx/cms/download_db.h"
#include "onyx/cms/cms_utils.h"

namespace cms
{


DownloadItem::DownloadItem()
    : state(STATE_INVALID)
    , time_stamp(QDateTime::currentDateTime().toString(dateFormat()))
{

}

DownloadItem::DownloadItem(const QString &u)
: url(u)
, path("")
, state(STATE_INVALID)
, time_stamp(QDateTime::currentDateTime().toString(dateFormat()))
{

}

DownloadItem::~DownloadItem()
{
}

bool DownloadItem::operator == (const DownloadItem &right)
{
    return url == right.url;
}


DownloadDB::DownloadDB()
{
    open();
}

DownloadDB::~DownloadDB()
{
    close();
}

bool DownloadDB::open()
{
    if (!database_)
    {
        database_.reset(new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE", "download.db")));
    }

    if (!database_->isOpen())
    {
        QDir home = QDir::home();
        database_->setDatabaseName(home.filePath("download.db"));
        if (!database_->open())
        {
            qDebug() << database_->lastError().text();
            return false;
        }
        makeSureTableExist(*database_);
    }
    return true;
}

bool DownloadDB::close()
{
    if (database_)
    {
        database_->close();
        database_.reset(0);
        QSqlDatabase::removeDatabase("download.db");
        return true;
    }
    return false;
}

DownloadList DownloadDB::pendingList(const QStringList & input,
                                     bool force_all,
                                     bool sort)
{
    DownloadList list;

    // Fetch all download items.
    QSqlQuery query(db());
    query.prepare( "select url, path, state, time_stamp from download ");
    if (!query.exec())
    {
        return list;
    }

    DownloadItem item;
    while (query.next())
    {
        item.url = query.value(0).toString();
        item.path = query.value(1).toString();
        item.state = static_cast<DownloadState>(query.value(2).toInt());
        item.time_stamp = query.value(3).toString();

        // Ignore items finished.
        if (item.state != FINISHED || force_all)
        {
            if (!list.contains(item))
            {
                list.push_back(item);
            }
        }
    }

    // check input now.
    foreach(QString i, input)
    {
        if (!list.contains(i))
        {
            list.push_back(i);
        }
    }

    if (sort)
    {
        qSort(list.begin(), list.end(), GreaterByTimestamp());
    }
    return list;
}

bool DownloadDB::updateState(const DownloadItem & item)
{
    QSqlQuery query(db());
    query.prepare( "INSERT OR REPLACE into download (url, path, state, time_stamp) values(?, ?, ?, ?)");
    query.addBindValue(item.url);
    query.addBindValue(item.path);
    query.addBindValue(item.state);
    query.addBindValue(item.time_stamp);
    return query.exec();
}

bool DownloadDB::updateState(const QString & url, DownloadState state)
{
    DownloadItem item;
    item.url = url;
    item.state = state;
    return updateState(item);
}

bool DownloadDB::makeSureTableExist(QSqlDatabase &db)
{
    QSqlQuery query(db);
    query.exec("create table if not exists download ("
               "url text primary key,"
               "path text,"
               "state integer,"
               "time_stamp text, "
               "value blob) ");
    query.exec("create index if not exists url_index on info (url)");
    return true;
}

QSqlDatabase & DownloadDB::db()
{
    return *database_;
}


}  // namespace cms


