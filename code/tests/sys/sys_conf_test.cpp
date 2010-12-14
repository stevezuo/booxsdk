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

TEST(SysConfTest, FontConfig)
{
    SystemConfig conf;
    {
        QString b("song");
        conf.setDefaultFontFamily(b);
        QString a = conf.defaultFontFamily();
        EXPECT_STREQ(b.toStdString().c_str() ,a.toStdString().c_str() ); 
    }
    {
        QString b("hei");
        conf.setDefaultFontFamily(b);
        QString a = conf.defaultFontFamily();
        EXPECT_STREQ(b.toStdString().c_str() ,a.toStdString().c_str()); 
    }
    {
        QString b("song");
        conf.setDefaultFontFamily(b);
        QString a = conf.defaultFontFamily();
        EXPECT_STREQ(b.toStdString().c_str() ,a.toStdString().c_str()); 
    }
    {
        QString b("song");
        QString a = conf.defaultFontFamily();
        EXPECT_TRUE(a == b);
    }
}

TEST(SysConfTest, MiscConfig)
{
    SystemConfig conf;
    static const QString KEY = "key1";
    static const QString VALUE = "value1";
    static const QString KEY2 = "key2";
    static const QString VALUE2 = "value2";
    conf.setMiscValue(KEY, VALUE);
    QString actual = conf.miscValue(KEY);
    EXPECT_TRUE(VALUE == actual);

    conf.setMiscValue(KEY2, VALUE2);
    actual = conf.miscValue(KEY2);
    EXPECT_TRUE(VALUE2 == actual);

    conf.setMiscValue(KEY, "");
    actual = conf.miscValue(KEY);
    EXPECT_TRUE(VALUE != actual);
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

TEST(SysConfTest, DialupConf)
{
    SystemConfig conf;
    DialupProfiles all;
    conf.loadDialupProfiles(all);

    DialupProfile a;
    a.setDisplayName("china unicom");
    a.setApn("unicom");
    all.push_back(a);
    conf.saveDialupProfiles(all);

    DialupProfiles result;
    conf.loadDialupProfiles(result);
    EXPECT_TRUE(all == result);
}

}
