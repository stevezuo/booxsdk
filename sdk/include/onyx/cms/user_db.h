#ifndef CMS_USER_DB_H_
#define CMS_USER_DB_H_

#include <QtGui/QtGui>
#include <QtSql/QtSql>
#include "onyx/base/base.h"

namespace cms
{

/// Store user information.
class UserDB
{
public:
    explicit UserDB(bool open = true);
    ~UserDB(void);

    bool open();
    bool close();

    bool store(const QString & tag, const QVariant & value);
    bool load(const QString & tag, QVariant & value);

private:
    bool makeSureTableExist(QSqlDatabase &db);

private:
    scoped_ptr<QSqlDatabase> database_;
};

}

#endif
