#ifndef DATABASE_VERSION_H_
#define DATABASE_VERSION_H_

#include "content_node.h"

namespace cms
{

/// Represent database version.
class Version
{
    friend class ContentManager;
public:
    Version(void);
    ~Version(void);

private:
    static bool makeSureTableExist(QSqlDatabase &);
    static bool getVersion(QSqlDatabase &, unsigned int &, unsigned int &);
};

}

#endif
