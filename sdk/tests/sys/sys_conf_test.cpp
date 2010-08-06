#include <QtGui/QtGui>
#include <cassert>

#include "onyx/base/qt_support.h"
#include "onyx/sys/sys_conf.h"
#include "gtest/gtest.h"

using namespace sys;

namespace
{

TEST(SysConfTest, PMConf)
{
    SystemConfig conf;
    static const int SUSPEND = rand();
    EXPECT_TRUE(conf.setSuspendInterval(SUSPEND));
    EXPECT_TRUE(SUSPEND == conf.suspendInterval());

    static const int SHUTDOWN = rand();
    EXPECT_TRUE(conf.setShutdownInterval(SHUTDOWN));
    EXPECT_TRUE(SHUTDOWN == conf.shutdownInterval());
}

/*
TEST(SysConfTest, LocaleConf)
{
    SystemConfig conf;
    static const QLocale LOCALE("zh_CN");
    EXPECT_TRUE(conf.setLocale(LOCALE ));
    EXPECT_TRUE(LOCALE  == conf.locale());
}
*/

TEST(SysConfTest, ServiceConf)
{
    SystemConfig conf;
    Service dummy(".dummy", ".dummy", ".dummy", ".dummy", ".dummy");
    dummy.mutable_extensions().push_back(".dummy");
    EXPECT_TRUE(conf.registerService(dummy, "dummy"));
    EXPECT_TRUE(conf.unRegisterService(dummy));
}


TEST(SysConfTest, VolumeConf)
{
    SystemConfig conf;
    int volume = rand();
    EXPECT_TRUE(conf.setVolume(volume));
    EXPECT_TRUE(volume == conf.volume());

    bool mute = false;
    EXPECT_TRUE(conf.mute(mute));
    EXPECT_TRUE(mute == conf.isMute());

    mute = true;
    EXPECT_TRUE(conf.mute(mute));
    EXPECT_TRUE(mute == conf.isMute());
}

TEST(SysConfTest, WifiConf)
{
    SystemConfig conf;
    EXPECT_TRUE(conf.clearWifiProfiles());

    WifiProfile a;
    static const QString NAME = "p1";
    static const QString KEY  = "key";
    static const QString PROTOCOL  = "protocol";
    a[NAME] = NAME;
    a[KEY] = KEY;
    a[PROTOCOL] = PROTOCOL;

    WifiProfiles all;
    all.push_back(a);
    conf.saveWifiProfiles(all);

    all.clear();
    conf.loadWifiProfiles(all);
    EXPECT_TRUE(all.size() == 1);
    EXPECT_TRUE(all.front() == a);
}

/*
TEST(SysConfTest, NotesTemplate)
{
    for(int i = -1; i < 10; ++i)
    {
        QString t = SystemConfig::notesTemplate(i);
        EXPECT_FALSE(t.isEmpty());
    }
}
*/

TEST(SysConfTest, deviceID)
{
    SystemConfig conf;
    QString q = conf.deviceId();
    QString p = conf.deviceId();
    EXPECT_TRUE(q == p);

    QString a = conf.serialNumber();
    QString b = conf.serialNumber();
    EXPECT_TRUE(a == b);
}

/*
TEST(SysConfTest, noteExportDir)
{
    QDir da, db;
    EXPECT_TRUE(SystemConfig::notesExportDirectory("a", da));
    EXPECT_TRUE(SystemConfig::notesExportDirectory("b", db));
    EXPECT_NE(da.absolutePath(), db.absolutePath());
}
*/

}

