// Authors: John

/// Public header of the system configuration library.

#ifndef SYS_VOLUME_CONF_H__
#define SYS_VOLUME_CONF_H__

#include <QtGui/QtGui>
#include <QtSql/QtSql>
#include "onyx/base/base.h"

namespace sys
{

/// Used by system manager only.
class VolumeConfig
{
    friend class SystemConfig;
public:
    VolumeConfig();
    ~VolumeConfig();

private:
    static bool makeSureTableExist(QSqlDatabase &);

    static int volume(QSqlDatabase &);
    static bool setVolume(QSqlDatabase&, int volume);

    static QVector<int> volumes();
    static int minVolume();
    static int maxVolume();

    static bool mute(QSqlDatabase &, bool mute);
    static bool isMute(QSqlDatabase &);

};

};

#endif  // SYS_FONT_CONF_H__
