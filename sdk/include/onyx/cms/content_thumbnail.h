#ifndef CMS_CONTENT_THUMBNAIL_H_
#define CMS_CONTENT_THUMBNAIL_H_

#include <QtGui/QtGui>
#include <QtSql/QtSql>
#include "onyx/base/base.h"

namespace cms
{


enum ThumbnailType
{
    THUMBNAIL_SMALL,
    THUMBNAIL_MIDDLE,
    THUMBNAIL_LARGE,
    THUMBNAIL_HUGE
};

bool isImage(const QString& suffix);

QSize thumbnailSize(ThumbnailType type = THUMBNAIL_LARGE);

bool makeThumbnail(const QString & path, ThumbnailType type = THUMBNAIL_LARGE);

/// Store all content file thumbnails under certain folder.
class ContentThumbnail
{
public:
    explicit ContentThumbnail(const QString &folder, bool open = true);
    ~ContentThumbnail(void);

    bool open();
    bool close();

    bool hasThumbnail(const QString & file, ThumbnailType type);
    bool loadThumbnail(const QString & file, ThumbnailType type, QImage & thumbnail);
    bool storeThumbnail(const QString & file, ThumbnailType type, const QImage & thumbnail);

    static const QString & databaseName();

private:
    bool makeSureTableExist(QSqlDatabase &db);

private:
    scoped_ptr<QSqlDatabase> database_;
    QDir dir_;
    static const QString DB_NAME;
};

inline const QString & ContentThumbnail::databaseName()
{
    return DB_NAME;
}

}

#endif
