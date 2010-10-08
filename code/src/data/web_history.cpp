#include <stdlib.h>
#include "onyx/cms/content_thumbnail.h"
#include "onyx/data/web_history.h"

using namespace cms;

namespace webhistory
{


WebHistory::WebHistory()
: max_count_(20)
{
    open("web_history");
    makeSureTableExist(*database_);
}

WebHistory::~WebHistory()
{
    close();
}

/// The database is in home directory.
bool WebHistory::open(const QString & app_name)
{
    if (!database_)
    {
        database_.reset(new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE", "application_config")));
        QString path("%1.db");
        path = path.arg(app_name);
        database_->setDatabaseName(QDir::home().filePath(path));
    }
    return database_->open();
}

bool WebHistory::close()
{
    if (database_)
    {
        database_->close();
        database_.reset(0);
        QSqlDatabase::removeDatabase("application_config");
        return true;
    }
    return false;
}

void WebHistory::makeSureTableExist(QSqlDatabase& database)
{
    QSqlQuery query(database);
    query.exec("create table if not exists conf ("
                      "key integer primary key, "
                      "value blob "
                      ")");
}

/// Load configuration from database.
bool WebHistory::loadConf(QVariantList & conf)
{
    QSqlQuery query(*database_);
    query.prepare( "select value from conf");

    if (!query.exec())
    {
        return false;
    }

    while (query.next())
    {
        QVariant var;
        QByteArray data = query.value(0).toByteArray();
        QBuffer buffer(&data);
        buffer.open(QIODevice::ReadOnly);
        QDataStream stream(&buffer);
        stream >> var;
        conf.append(var);
    }
    return true;
}

bool WebHistory::clear(QSqlDatabase& database)
{
    QSqlQuery query(database);
    query.prepare( "delete from conf");
    if (!query.exec())
    {
        qDebug() << query.lastError().text();
        return false;
    }
    return true;
}

bool WebHistory::saveConf(const QVariantList & conf)
{
    clear(*database_);

    QSqlQuery query(*database_);

    query.prepare( "insert into conf (value) values(?)");

    // We only store the first maxCount pages.
    int index = 0;
    foreach(QVariant v, conf)
    {
        QByteArray data;
        QBuffer buffer(&data);
        buffer.open(QIODevice::WriteOnly);
        QDataStream stream(&buffer);
        stream << v;
        query.addBindValue(data);
        query.exec();

        // Check
        if (++index >= maxCount())
        {
            break;
        }
    }
    return true;
}

void WebHistory::clear()
{
    clear(*database_);
}

ThumbnailItem::ThumbnailItem()
    : QMap<QString, QVariant>()
{
}

ThumbnailItem::ThumbnailItem( const QMap<QString, QVariant>& right )
    : QMap<QString, QVariant> ( right )
{
}

ThumbnailItem::~ThumbnailItem()
{
}

QSize ThumbnailItem::size()
{
    return thumbnailSize(THUMBNAIL_HUGE);
}

void ThumbnailItem::toByteArray(QByteArray & data)
{
    QBuffer buffer(&data);
    buffer.open(QIODevice::WriteOnly);
    QDataStream stream(&buffer);
    stream << *this;
}

void ThumbnailItem::fromByteArray(QByteArray & data)
{
    QBuffer buffer(&data);
    buffer.open(QIODevice::ReadOnly);
    QDataStream stream(&buffer);
    stream >> *this;
}

const QString ThumbnailItem::title()
{
     return value("title").toString();
}

void ThumbnailItem::setTitle(const QString & title)
{
    insert("title", title);
}

const QUrl ThumbnailItem::url()
{
    return value("url").toUrl();
}

void ThumbnailItem::setUrl(const QUrl & url)
{
    insert("url", url);
}

QImage ThumbnailItem::thumbnail()
{
    QImage t;
    t.loadFromData(value("thumbnail").toByteArray());
    return t;
}

void ThumbnailItem::setThumbnail(const QImage & image)
{
    QByteArray ba;
    QBuffer buffer(&ba);
    buffer.open(QIODevice::WriteOnly);
    image.save(&buffer, "png");
    insert("thumbnail", ba);
}

void ThumbnailItem::updateAccessTime()
{
    setAccessTime(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm"));
}

QString ThumbnailItem::accessTime()
{
    return value("access").toString();
}

void ThumbnailItem::setAccessTime(const QString & time)
{
    insert("access", time);
}

bool GreaterByAccessTime::operator()(const QVariant& a, const QVariant& b) const
{
    return (a.toMap().value("access").toString() > b.toMap().value("access").toString());
}



}

