// Authors: John

/// Public header of the system configuration library.

#ifndef SYS_PAGE_TURNING_CONF_H__
#define SYS_PAGE_TURNING_CONF_H__

#include <QtGui/QtGui>
#include <QtSql/QtSql>
#include "onyx/base/base.h"


namespace sys
{

/// Page turning direction configuration. The settings are defined
/// as following:
/// >0 From left to right or bottom to top.
/// == 0 Don't distance is too small.
/// <0 From right to left or top to bottom.
class PageTurningConfig
{
public:
    PageTurningConfig();
    ~PageTurningConfig();

private:
    friend class SystemConfig;
    static bool makeSureTableExist(QSqlDatabase& db);
    static bool load(QSqlDatabase& db);
    static bool save(QSqlDatabase& db);

    static int direction(const QPoint & old_position, const QPoint & new_position);
    static void setDirection(int);
    static int distance();

private:
    static int direction_;
    static int THRESHOLD;
};

};

#endif  // SYS_PAGE_TURNING_CONF_H__
