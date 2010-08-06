#include "onyx/cms/content_bookmarks.h"

namespace cms
{

ContentBookmarks::ContentBookmarks(void)
{
}

ContentBookmarks::~ContentBookmarks(void)
{
}

bool ContentBookmarks::makeSureTableExist(QSqlDatabase &database)
{
    QSqlQuery query(database);
    return query.exec( "create table if not exists content_bookmarks ("
                       "id integer primary key, "
                       "bookmarks blob)");

    // May also need to create a index.
}

bool ContentBookmarks::getBookmarks(QSqlDatabase &database,
                                    const cms_long id,
                                    cms_blob & data)
{
    QSqlQuery query(database);
    query.prepare( "select bookmarks from content_bookmarks where id = ? ");
    query.addBindValue(id);

    if (query.exec() && query.next())
    {
        data = query.value(0).toByteArray();
        return true;
    }
    return false;
}

bool ContentBookmarks::createBookmarks(QSqlDatabase &database,
                                       const cms_long id,
                                       const cms_blob & bookmarks)
{
    QSqlQuery query(database);

    // Check the option is already exist or not.
    query.prepare( "select id from content_bookmarks where id = ? ");
    query.addBindValue(id);
    if (query.exec() && query.record().count())
    {
        return false;
    }

    query.prepare("insert into content_bookmarks (id, bookmarks) values(?, ?) ");
    query.addBindValue(id);
    query.addBindValue(bookmarks);
    return query.exec();
}

bool ContentBookmarks::removeBookmarks(QSqlDatabase &database,
                                       const cms_long id)
{
    QSqlQuery query(database);
    query.prepare( "delete from content_bookmarks where id = ?");
    query.addBindValue(id);
    return query.exec();
}

bool ContentBookmarks::updateBookmarks(QSqlDatabase &database,
                                       const cms_long id,
                                       const cms_blob & bookmarks)
{
    QSqlQuery query(database);
    query.exec("INSERT OR REPLACE into content_bookmarks (id, bookmarks) values(?, ?)");
    query.addBindValue(id);
    query.addBindValue(bookmarks);
    return query.exec();
}

bool ContentBookmarks::removeTable(QSqlDatabase &database)
{
    QSqlQuery query(database);
    return query.exec( "drop table content_bookmarks" );
}
}
