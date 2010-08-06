#ifndef CMS_CONTENT_BOOKMARKS_H_
#define CMS_CONTENT_BOOKMARKS_H_

#include "content_node.h"

namespace cms
{

/// Use content id as the primary key.
class ContentBookmarks
{
    friend class ContentManager;
public:
    ContentBookmarks(void);
    ~ContentBookmarks(void);

private:
    static bool makeSureTableExist(QSqlDatabase &);
    static bool removeTable(QSqlDatabase &database);

    static bool getBookmarks(QSqlDatabase &, const cms_long, cms_blob &);
    static bool createBookmarks(QSqlDatabase &, const cms_long, const cms_blob &);
    static bool removeBookmarks(QSqlDatabase &, const cms_long);
    static bool updateBookmarks(QSqlDatabase &, const cms_long, const cms_blob &);
};

}

#endif  // CONTENT_BOOKMARKS_H_
