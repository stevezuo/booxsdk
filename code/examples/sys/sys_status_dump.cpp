#include <QtGui/QtGui>
#include <cassert>

#include "sys_status_dump.h"

SysStatusUser::SysStatusUser(SysStatus& ref)
: sys_status(ref)
{
    installSysStatusSlot();

    testPMIntervals();

    testVolume();

    testDRMService();
}

SysStatusUser::~SysStatusUser()
{
}

void SysStatusUser::installSysStatusSlot()
{
    SysStatus& sys_status = SysStatus::instance();
    connect(&sys_status,
            SIGNAL(mountTreeSignal(bool, const QString&)),
            this,
            SLOT(processMountTreeEvent(bool,  const QString&)));
    connect(&sys_status,
            SIGNAL(connectToPC(bool)),
            this,
            SLOT(processConnectToPC(bool)));
    connect(&sys_status,
            SIGNAL(batterySignal(int, int)),
            this,
            SLOT(processBatteryEvent(int, int)));
}

void SysStatusUser::testPMIntervals()
{
    static const int SUSPEND = rand();
    sys_status.setSuspendInterval(SUSPEND);
    assert(sys_status.suspendInterval() == SUSPEND);

    static const int SHUTDOWN = rand();
    sys_status.setShutdownInterval(SHUTDOWN);
    assert(sys_status.shutdownInterval() == SHUTDOWN);
}

void SysStatusUser::processMountTreeEvent(bool inserted, const QString &mount_point)
{
    if (inserted)
    {
        qDebug("%s inserted!", qPrintable(mount_point));
    }
    else
    {
        qDebug("%s gone!", qPrintable(mount_point));
    }
}

void SysStatusUser::processConnectToPC(bool connected)
{
    if (connected)
    {
        qDebug("Connect to PC");
    }
    else
    {
        qDebug("Disconnect from pc");
    }
}

void SysStatusUser::processBatteryEvent(int current,
                                        int status)
{
    qDebug("current battery value %d status %d", current, status);
}

void SysStatusUser::testVolume()
{
    const int v = rand();
    sys_status.setVolume(v);
    sys_status.volume();

    bool mute = false;
    sys_status.mute(mute);
    sys_status.isMute();
}

void SysStatusUser::testDRMService()
{
    sys_status.addDRMEnvironment();
    QStringList all = QProcess::systemEnvironment();
    foreach(QString a, all)
    {
        qDebug("env entry %s", qPrintable(a));
    }
    sys_status.startDRMService(QStringList());
}

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    SysStatus & sys_status = SysStatus::instance();
    SysStatusUser sys_status_user(sys_status);
    sys_status.dump();

    return app.exec();
}
