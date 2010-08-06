#include "onyx/cms/cms_utils.h"
#include "onyx/data/database.h"

using namespace cms;

DataBase::DBMap DataBase::db_map_;

static bool getDBNameByDocName( const QString doc_name,
                                QString & db_name )
{
    db_name = getSketchDB(doc_name);
    return !db_name.isEmpty();
}

DataBase::DataBase( const QString & doc_name )
{
    if (getDBNameByDocName(doc_name, db_name_))
    {
        open();
    }
}

DataBase::~DataBase()
{
    close();
}

void DataBase::open()
{
    if (database_ != 0 || db_name_.isEmpty())
    {
        return;
    }

    database_.reset(new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE", db_name_)));
    if (database_ != 0)
    {
        database_->setDatabaseName(db_name_);
        if (!database_->open())
        {
            qDebug() << database_->lastError().text();
        }
    }
}

void DataBase::close()
{
    if (database_)
    {
        database_->close();
        database_.reset(0);
        QSqlDatabase::removeDatabase(db_name_);
    }
}

shared_ptr<DataBase> DataBase::getDB( const QString & doc_name )
{
    DBMapIter pos = db_map_.find( doc_name );
    if ( pos != db_map_.end() )
    {
        shared_ptr<DataBase> exist_db = pos.value();
        if (exist_db->database_ == 0)
        {
            exist_db->open();
        }
        return exist_db;
    }

    // construct a new IO instance
    shared_ptr<DataBase> db( new DataBase( doc_name ) );
    db_map_[doc_name] = db;
    return db;
}

bool DataBase::isDBExist( const QString & doc_name )
{
    QString db_name;
    if ( !getDBNameByDocName( doc_name, db_name ) )
    {
        return false;
    }

    QFileInfo db_info(db_name);
    if (db_info.exists())
    {
        return true;
    }
    return false;
}

