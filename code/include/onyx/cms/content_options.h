#ifndef CMS_CONTENT_OPTIONS_H_
#define CMS_CONTENT_OPTIONS_H_

#include "content_node.h"


namespace cms
{

/// Use content id as the primary key.
class ContentOptions
{
    friend class ContentManager;
public:
    ContentOptions(void);
    ~ContentOptions(void);

private:
    static bool makeSureTableExist(QSqlDatabase &);
    static bool removeTable(QSqlDatabase &database);

    static bool getOptions(QSqlDatabase &, const cms_long, cms_blob &);
    static bool createOptions(QSqlDatabase &, const cms_long, const cms_blob &);
    static bool removeOptions(QSqlDatabase &, const cms_long);
    static bool updateOptions(QSqlDatabase &, const cms_long, const cms_blob &);
};

}

#endif
