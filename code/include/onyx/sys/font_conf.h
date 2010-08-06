// Authors: John

/// Public header of the system configuration library.

#ifndef SYS_FONT_CONF_H__
#define SYS_FONT_CONF_H__

#include <QtGui/QtGui>
#include <QtSql/QtSql>
#include "onyx/base/base.h"



namespace sys
{

class FontConfig
{
public:
    FontConfig();
    ~FontConfig();

private:
    QString getFontFamily(QSqlDatabase &, QFontDatabase::WritingSystem);
    void setFontFamily(QSqlDatabase&, QFontDatabase::WritingSystem, const QString&);

    bool installFont(QSqlDatabase&, const QString &);
    bool removeFont(QSqlDatabase&, const QString &);

    void reset(QSqlDatabase&);
};

};

#endif  // SYS_FONT_CONF_H__
