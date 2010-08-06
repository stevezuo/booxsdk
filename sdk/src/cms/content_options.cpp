
#include "onyx/cms/content_options.h"

namespace cms
{

ContentOptions::ContentOptions(void)
{
}

ContentOptions::~ContentOptions(void)
{
}

bool ContentOptions::makeSureTableExist(QSqlDatabase &database)
{
    QSqlQuery query(database);
    return query.exec( "create table if not exists content_options ("
                       "id integer primary key, "
                       "options blob)");
}

bool ContentOptions::getOptions(QSqlDatabase &database,
                                const cms_long id,
                                cms_blob & data)
{
    QSqlQuery query(database);
    query.prepare( "select options from content_options where id = ? ");
    query.addBindValue(id);

    if (query.exec() && query.next())
    {
        data = query.value(0).toByteArray();
        return true;
    }
    return false;
}

bool ContentOptions::createOptions(QSqlDatabase &database,
                                   const cms_long id,
                                   const cms_blob & options)
{
    QSqlQuery query(database);

    // Check the option is already exist or not.
    query.exec( "select id from content_options where id = ? ");
    query.addBindValue(id);
    if (query.exec() && query.record().count())
    {
        return false;
    }

    query.prepare("insert into content_options (id, options) values(?, ?)");
    query.addBindValue(id);
    query.addBindValue(options);
    return query.exec();
}

bool ContentOptions::removeOptions(QSqlDatabase &database,
                                   const cms_long id)
{
    QSqlQuery query(database);
    query.prepare( "delete from content_options where id = ?");
    query.addBindValue(id);
    return query.exec();
}

bool ContentOptions::updateOptions(QSqlDatabase &database,
                                   const cms_long id,
                                   const cms_blob & options)
{
    QSqlQuery query(database);
    query.prepare( "INSERT OR REPLACE into content_options (id, options) values(?, ?)");
    query.addBindValue(id);
    query.addBindValue(options);
    return query.exec();
}

bool ContentOptions::removeTable(QSqlDatabase &database)
{
    QSqlQuery query(database);
    return query.exec( "drop table content_options" );
}

}
