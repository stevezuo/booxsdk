// Author: John

#ifndef CMS_DOWNLOAD_DB_H_
#define CMS_DOWNLOAD_DB_H_

#include <QtCore/QtCore>
#include <QtSql/QtSql>
#include "onyx/base/base.h"

namespace cms
{

enum DownloadState
{
    STATE_INVALID = -1,
    DOWNLOADING,
    FINISHED,
    FAILED,
};

struct DownloadItem
{
public:
    DownloadItem();
    DownloadItem(const QString &url);
    ~DownloadItem();

    bool operator == (const DownloadItem &right);

public:
    QString url;        ///< Download url.
    QString path;       ///< Local file path.
    DownloadState state;    ///< Download state.
    QString time_stamp;
};
typedef QVector<DownloadItem> DownloadList;

struct LessByUrl
{
    bool operator()( const DownloadItem & a, const DownloadItem & b ) const
    {
        return (a.url.compare(b.url, Qt::CaseInsensitive) < 0);
    }
};

struct GreaterByUrl
{
    bool operator()( const DownloadItem & a, const DownloadItem & b ) const
    {
        return (a.url.compare(b.url, Qt::CaseInsensitive) > 0);
    }
};

struct LessByTimestamp
{
    bool operator()( const DownloadItem & a, const DownloadItem & b ) const
    {
        return (a.time_stamp.compare(b.time_stamp, Qt::CaseInsensitive) < 0);
    }
};

struct GreaterByTimestamp
{
    bool operator()( const DownloadItem & a, const DownloadItem & b ) const
    {
        return (a.time_stamp.compare(b.time_stamp, Qt::CaseInsensitive) > 0);
    }
};

class DownloadDB
{
public:
    DownloadDB();
    ~DownloadDB();

public:
    bool open();
    bool close();

    DownloadList pendingList(const QStringList & input = QStringList(),
                             bool force_all = false,
                             bool sort = true);

    bool updateState(const DownloadItem & item);
    bool updateState(const QString & url, DownloadState state = FINISHED);

private:
    bool makeSureTableExist(QSqlDatabase &db);
    QSqlDatabase & db();

private:
    scoped_ptr<QSqlDatabase> database_;

};


}  // namespace cms

#endif  // CMS_DOWNLOAD_DB_H_
