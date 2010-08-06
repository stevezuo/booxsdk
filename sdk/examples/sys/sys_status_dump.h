#ifndef SYS_STATUS_TEST_H_
#define SYS_STATUS_TEST_H_

#include <QObject>
#include "onyx/sys/sys.h"

class SysStatusUser : public QObject
{
    Q_OBJECT

public:
    SysStatusUser(SysStatus& ref);
    ~SysStatusUser();

private Q_SLOTS:
    void processMountTreeEvent(bool inserted, const QString &mount_point);
    void processConnectToPC(bool connected);
    void processBatteryEvent(int current, int);

private:
    void installSysStatusSlot();
    void testPMIntervals();
    void testVolume();
    void testDRMService();

private:
    SysStatus& sys_status;
};

#endif // SYS_STATUS_TEST_H_
