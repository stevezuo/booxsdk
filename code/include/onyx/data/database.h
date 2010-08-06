#ifndef DATABASE_H_
#define DATABASE_H_

#include <QtSql/QtSql>
#include "onyx/base/base.h"
#include "onyx/base/down_cast.h"

class DataBase
{
public:
    DataBase( const QString & doc_name );
    ~DataBase();

    inline QSqlDatabase* database();
    inline const QString & name();

    void open();
    void close();

    static bool isDBExist( const QString & doc_name );
    static shared_ptr<DataBase> getDB( const QString & doc_name );

private:
    typedef QMap< QString, shared_ptr<DataBase> > DBMap;
    typedef DBMap::iterator                       DBMapIter;

private:
    scoped_ptr<QSqlDatabase> database_;    ///< sqlite qt wrapper.
    QString                  db_name_;     ///< Connection name of the db
    static DBMap             db_map_;      ///< map of all the db instances
};

QSqlDatabase* DataBase::database()
{
    if (database_ != 0)
    {
        return database_.get();
    }
    open();
    return database_.get();
}

const QString & DataBase::name()
{
    return db_name_;
}

#endif
