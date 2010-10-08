// Authors: John

/// Public header of the application configuration library.

#ifndef WEB_HISTORY_H_
#define WEB_HISTORY_H_


#include "onyx/base/base.h"
#include <QtCore/QtCore>
#include <QtSql/QtSql>

namespace webhistory
{

class WebHistory
{
public:
    WebHistory();
    ~WebHistory();

public:
    // conf.
    bool loadConf(QVariantList & conf);
    bool saveConf(const QVariantList & conf);
    void clear();

    void setMaxCount(int max) { max_count_ = max; }
    int  maxCount() { return max_count_; }

private:
    bool open(const QString & app_name);
    bool close();
    void makeSureTableExist(QSqlDatabase& database);
    bool clear(QSqlDatabase& database);

private:
    scoped_ptr<QSqlDatabase> database_;
    int max_count_;
};


class ThumbnailItem : public QMap<QString, QVariant>
{
public:
    ThumbnailItem();
    ThumbnailItem( const QMap<QString, QVariant>& right );
    ~ThumbnailItem();

public:
    static QSize size();

    void toByteArray(QByteArray & data);
    void fromByteArray(QByteArray & data);

    const QString title();
    void setTitle(const QString & title);

    const QUrl url();
    void setUrl(const QUrl & u);

    QImage thumbnail();
    void setThumbnail(const QImage & image);

    void updateAccessTime();
    QString accessTime();
    void setAccessTime(const QString & time);
};

struct GreaterByAccessTime
{
    bool operator()( const QVariant& a, const QVariant& b) const;
};

};

#endif  // WEB_HISTORY_H_
