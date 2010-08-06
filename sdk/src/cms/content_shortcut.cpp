#include "onyx/cms/content_shortcut.h"

namespace cms
{

ContentShortcut::ContentShortcut(void)
{
}

ContentShortcut::~ContentShortcut(void)
{
}

bool ContentShortcut::makeSureTableExist(QSqlDatabase &database)
{
    QSqlQuery query(database);
    return query.exec( "create table if not exists content_shortcuts ("
                       "id integer primary key, "
                       "src text, "
                       "target text)");

    // May also need to create a index.
}

QString ContentShortcut::target(QSqlDatabase & database,
                                const QString & src)
{
    QSqlQuery query(database);
    query.prepare( "select target from content_shortcuts where src = ?");
    query.addBindValue(src);

    QString ret;
    if (query.exec() && query.next())
    {
        ret = query.value(0).toString();
    }
    return ret;
}

/// Return how many links with source src.
int ContentShortcut::links(QSqlDatabase & database,
                           const QString & src)
{
    QStringList srcs, targets;
    all(database, srcs, targets);
    return srcs.count(src);
}

bool ContentShortcut::link(QSqlDatabase &database,
                           const QString &src,
                           const QString &target)
{
    QSqlQuery query(database);
    query.prepare( "INSERT OR REPLACE into content_shortcuts (src, target) values(?, ?)");
    query.addBindValue(src);
    query.addBindValue(target);
    return query.exec();
}

bool ContentShortcut::unlinkBySource(QSqlDatabase & database,
                                     const QString &src)
{
    QSqlQuery query(database);
    query.prepare( "delete from content_shortcuts where src = ?");
    query.addBindValue(src);
    return query.exec();
}

bool ContentShortcut::unlinkByTarget(QSqlDatabase & database,
                                     const QString &target)
{
    QSqlQuery query(database);
    query.prepare( "delete from content_shortcuts where target = ?");
    query.addBindValue(target);
    return query.exec();
}

bool ContentShortcut::all(QSqlDatabase & database,
                          QStringList & srcs,
                          QStringList & targets)
{
    QSqlQuery query(database);
    query.prepare( "select src, target from content_shortcuts ");

    if (!query.exec())
    {
        return false;
    }
    while (query.next())
    {
        srcs << query.value(0).toString();
        targets << query.value(1).toString();
    }
    return true;
}

}
