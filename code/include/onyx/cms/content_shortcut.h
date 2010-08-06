#ifndef CMS_CONTENT_SHORTCUT_H_
#define CMS_CONTENT_SHORTCUT_H_

#include <QtGui/QtGui>
#include <QtSql/QtSql>
#include "onyx/base/base.h"

namespace cms
{

/// Store all content shortcut links. The table records link from source to target
/// One source could be mapped to different targets.
class ContentShortcut
{
    friend class ContentManager;
public:
    ContentShortcut(void);
    ~ContentShortcut(void);

private:
    static bool makeSureTableExist(QSqlDatabase &);
    static bool removeTable(QSqlDatabase &database);

    static QString target(QSqlDatabase &, const QString &);
    static int  links(QSqlDatabase &, const QString &src);
    static bool link(QSqlDatabase &, const QString &src, const QString &target);
    static bool unlinkBySource(QSqlDatabase &, const QString &src);
    static bool unlinkByTarget(QSqlDatabase &, const QString &target);
    static bool all(QSqlDatabase &, QStringList & srcs, QStringList & targets);
};

}

#endif
