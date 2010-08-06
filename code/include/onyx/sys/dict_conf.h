// Authors: John

/// Public header of the system configuration library.

#ifndef SYS_DICT_CONF_H__
#define SYS_DICT_CONF_H__

#include "onyx/base/base.h"
#include <QString>
#include <QtSql/QtSql>

namespace sys
{

class DictConfig
{
public:
    DictConfig();
    ~DictConfig();

private:
    friend class SystemConfig;

    static bool makeSureTableExist(QSqlDatabase& db);
    static bool dictionaryRoots(QSqlDatabase& db, QStringList & dirs);
    static QString selectedDictionary(QSqlDatabase& db);
    static bool selectDictionary(QSqlDatabase& db, const QString & name);

};

};

#endif  // SYS_LOCALE_CONF_H__
