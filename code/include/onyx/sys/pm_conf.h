// Authors: John

/// Public header of the system configuration library.

#ifndef SYS_POWER_MANAGEMENT_CONF_H__
#define SYS_POWER_MANAGEMENT_CONF_H__

#include <QtGui/QtGui>
#include <QtSql/QtSql>
#include "onyx/base/base.h"

namespace sys
{

/// Used by system manager only.
class PMConfig
{
    friend class SystemConfig;
public:
    PMConfig();
    ~PMConfig();

private:
    static bool makeSureTableExist(QSqlDatabase &);

    static int suspendInterval(QSqlDatabase &);
    static bool setSuspendInterval(QSqlDatabase&, int ms);

    static int shutdownInterval(QSqlDatabase &);
    static bool setShutdownInterval(QSqlDatabase&, int ms);


};

};

#endif  // SYS_FONT_CONF_H__
