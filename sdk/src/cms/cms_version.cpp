
#include "onyx/cms/cms_version.h"


namespace cms
{

/// TODO, define current version. The version from database could be different
/// from current version which should be always larger than version from db.
Version::Version(void)
{
}

Version::~Version(void)
{
}

bool Version::makeSureTableExist(QSqlDatabase &database)
{
    QSqlQuery query(database);
    query.exec( "create table if not exists version ("
                "major integer, "
                "minor integer"
                ")" );
    return true;
}

bool Version::getVersion(QSqlDatabase &database,
                         unsigned int &major,
                         unsigned int &minor)
{
    QSqlQuery query(database);
    query.prepare( "select major, minor from version" );
    if (query.exec()  && query.next())
    {
        major = query.value(0).toInt();
        minor = query.value(1).toInt();
        return true;
    }
    return false;
}

}

