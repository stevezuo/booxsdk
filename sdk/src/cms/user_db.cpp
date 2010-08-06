

#include <QList>
#include <QByteArray>
#include <QImageReader>
#include <QFileInfo>

#include "onyx/cms/cms_utils.h"
#include "onyx/cms/user_db.h"


namespace cms
{

/// Store user information.
UserDB::UserDB(bool open_db)
{
    if (open_db)
    {
        open();
    }
}

UserDB::~UserDB(void)
{
    close();
}

bool UserDB::open()
{
    if (!database_)
    {
        database_.reset(new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE", "user.db")));
    }

    if (!database_->isOpen())
    {
        QDir home = QDir::home();
        database_->setDatabaseName(home.filePath("user.db"));
        if (!database_->open())
        {
            qDebug() << database_->lastError().text();
            return false;
        }
        makeSureTableExist(*database_);
    }
    return true;
}

bool UserDB::close()
{
    if (database_)
    {
        database_->close();
        database_.reset(0);
        QSqlDatabase::removeDatabase("user.db");
        return true;
    }
    return false;
}

bool UserDB::store(const QString & tag, const QVariant & value)
{
    QSqlQuery query(*database_);
    query.prepare("INSERT OR REPLACE into info (name, value) VALUES (?, ?)");
    query.addBindValue(tag);
    query.addBindValue(value);
    if (!query.exec())
    {
        qDebug() << query.lastError().text();
        return false;
    }
    return true;
}

bool UserDB::load(const QString & tag, QVariant & value)
{
    QSqlQuery query(*database_);
    query.prepare("SELECT value FROM info where name = ?");
    query.addBindValue(tag);
    if (!query.exec())
    {
        qDebug() << query.lastError().text();
        return false;
    }

    if (query.next())
    {
        value = query.value(0);
        return true;
    }
    return false;
}

bool UserDB::makeSureTableExist(QSqlDatabase &db)
{
    QSqlQuery query(db);
    query.exec("create table if not exists info ("
               "name text primary key,"
               "value blob) ");
    query.exec("create index if not exists name_index on info (name)");
    return true;
}

}

