
#include <QtGui/QtGui>
#include "onyx/sys/sys.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    SysStatus & sys_status = SysStatus::instance();

    int left = 0;
    int status = BATTERY_STATUS_DANGEROUS;

    if (argc >= 3)
    {
        left = app.arguments().at(1).toInt();
        status = app.arguments().at(2).toInt();
    }

    sys_status.dbgUpdateBattery(left, status);
    return 0;
}
