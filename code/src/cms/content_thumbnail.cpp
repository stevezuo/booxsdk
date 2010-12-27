

#include <QList>
#include <QByteArray>
#include <QImageReader>
#include <QFileInfo>

#include "onyx/cms/content_thumbnail.h"
#include "onyx/cms/cms_utils.h"


namespace cms
{

const QString ContentThumbnail::DB_NAME = ".onyx.thumbs.db";

ContentThumbnail::ContentThumbnail(const QString &folder, bool open_db)
: database_()
, dir_(folder)
{
    if (open_db)
    {
        open();
    }
}

ContentThumbnail::~ContentThumbnail(void)
{
    close();
}

bool ContentThumbnail::open()
{
    if (!database_)
    {
        database_.reset(new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE",dir_.absolutePath())));
        database_->setDatabaseName(getThumbDB(dir_.absolutePath()));
    }

    if (!database_->isOpen())
    {
        if (!database_->open())
        {
            qDebug() << database_->lastError().text();
            return false;
        }
        makeSureTableExist(*database_);

        // TODO, it's better to make the file hidden.
    }
    return true;
}

bool ContentThumbnail::close()
{
    if (database_)
    {
        database_->close();
        database_.reset(0);
        QSqlDatabase::removeDatabase(dir_.absolutePath());
        return true;
    }
    return false;
}

/// Check if the database contains the specified type of thumbnail
/// or not.
bool ContentThumbnail::hasThumbnail(const QString & file_name,
                                    ThumbnailType type)
{
    QFileInfo info(dir_, file_name);

    QSqlQuery query(*database_);
    if (type == THUMBNAIL_SMALL)
    {
        query.prepare("SELECT small_image FROM thumbs where name = ? and date >= ?");
    }
    else if (type == THUMBNAIL_MIDDLE)
    {
        query.prepare("SELECT middle_image FROM thumbs where name = ? and date >= ?");
    }
    else if (type == THUMBNAIL_LARGE)
    {
        query.prepare("SELECT large_image FROM thumbs where name = ? and date >= ?");
    }
    query.addBindValue(file_name);
    QString t = info.lastModified().toString(Qt::ISODate);
    query.addBindValue(t);
    if (!query.exec())
    {
        qDebug() << query.lastError().text();
        return false;
    }

    // Should not use size here.
    return (query.next());
}

bool ContentThumbnail::loadThumbnail(const QString & file_name,
                                     ThumbnailType type,
                                     QImage & thumbnail)
{
    QFileInfo info(dir_, file_name);

    QSqlQuery query(*database_);
    if (type == THUMBNAIL_SMALL)
    {
        query.prepare("SELECT small_image FROM thumbs where name = ? and date >= ?");
    }
    else if (type == THUMBNAIL_MIDDLE)
    {
        query.prepare("SELECT middle_image FROM thumbs where name = ? and date >= ?");
    }
    else if (type == THUMBNAIL_LARGE)
    {
        query.prepare("SELECT large_image FROM thumbs where name = ? and date >= ?");
    }
    query.addBindValue(file_name);
    query.addBindValue(info.lastModified().toString(Qt::ISODate));
    if (!query.exec())
    {
        qDebug() << query.lastError().text();
        return false;
    }

    if (query.next())
    {
        return thumbnail.loadFromData(query.value(0).toByteArray());
    }
    return false;
}

/// Store the thumbnail of the file into the database.
/// @param file_name The file name not the absolute file path.
/// @param thumbnail The thumbnail image of the content file.
bool ContentThumbnail::storeThumbnail(const QString & file_name,
                                      ThumbnailType type,
                                      const QImage & thumbnail)
{
    QFileInfo info(dir_, file_name);

    QSqlQuery query(*database_);
    if (type == THUMBNAIL_SMALL)
    {
        query.prepare("INSERT OR REPLACE into thumbs (name, date, small_image) VALUES (?, ?, ?)");
    }
    else if (type == THUMBNAIL_MIDDLE)
    {
        query.prepare("INSERT OR REPLACE into thumbs (name, date, middle_image) VALUES (?, ?, ?)");
    }
    else if (type == THUMBNAIL_LARGE)
    {
        query.prepare("INSERT OR REPLACE into thumbs (name, date, large_image) VALUES (?, ?, ?)");
    }

    query.addBindValue(file_name);
    QString t = info.lastModified().toString(Qt::ISODate);
    query.addBindValue(t);

    QByteArray ba;
    QBuffer buffer(&ba);
    buffer.open(QIODevice::WriteOnly);

    // Have to store the image in the byte array with png format.
    // This function may be used by viewer to store their cover page,
    // so can not use suffix as format string.
    if (isImage(info.suffix()))
    {
        thumbnail.save(&buffer, info.suffix().toAscii());
    }
    else
    {
        thumbnail.save(&buffer, "PNG");
    }

    query.addBindValue(ba);
    if (!query.exec())
    {
        qDebug() << query.lastError().text();
        return false;
    }
    return true;
}

bool ContentThumbnail::makeSureTableExist(QSqlDatabase &db)
{
    QSqlQuery query(db);
    query.exec("create table if not exists thumbs ("
               "name text primary key,"
               "date text,"
               "small_image blob, "
               "middle_image blob, "
               "large_image blob)");
    query.exec("create index if not exists name_index on thumbs (name)");
    return true;
}



/// Check the suffix is a image suffix or not.
bool isImage(const QString& suffix)
{
    static QList<QString> supported_formats;
    if (supported_formats.size() <= 0)
    {
        QList<QByteArray> list = QImageReader::supportedImageFormats();
        for(QList<QByteArray>::iterator it = list.begin(); it != list.end(); ++it)
        {
            QString ext(*it);
            ext = ext.toLower();
            supported_formats.push_back(ext);
        }
    }
    return supported_formats.contains(suffix.toLower());
}

/// Get thumbnail size according to the thumbnail type.
QSize thumbnailSize(ThumbnailType type)
{
    if (type == THUMBNAIL_SMALL)
    {
        return QSize(25, 25);
    }
    else if (type == THUMBNAIL_MIDDLE)
    {
        return QSize(84, 84);
    }
    else if (type == THUMBNAIL_LARGE)
    {
        return QSize(124, 124);
    }
    else if (type == THUMBNAIL_HUGE)
    {
        return QSize(256, 256);
    }
    return QSize();
}

bool makeThumbnail(const QString & path,
                   ThumbnailType type)
{
    QFileInfo info(path);
    ContentThumbnail db(info.path());
    QImage thumbnail;
    if (!db.loadThumbnail(path, THUMBNAIL_LARGE, thumbnail))
    {
        // So far, only support image thumbnail in explorer.
        // The viewer can generate thumbnail themselves.
        if (!isImage(info.suffix()))
        {
            return false;
        }

        // Try to load it if it's a image.
        if (!thumbnail.load(path))
        {
            return false;
        }

        thumbnail = thumbnail.scaled(thumbnailSize(THUMBNAIL_LARGE), Qt::KeepAspectRatio);
        db.storeThumbnail(info.fileName(), THUMBNAIL_LARGE, thumbnail);

        // Return true even the thumbnail can not be stored.
        return true;
    }
    return true;

}

}

