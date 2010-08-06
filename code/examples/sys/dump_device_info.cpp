#include "onyx/sys/sys.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    sys::SystemConfig sys;
    qDebug("serial number %s", qPrintable(sys.serialNumber()));
    qDebug("device id %s", qPrintable(sys.deviceId()));
    qDebug("version %s", qPrintable(sys::SystemConfig::version()));
    return 0;
}

