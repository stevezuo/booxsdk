
#include "onyx/base/dbus.h"
#include "application.h"

using namespace dummy;

int main(int argc, char *argv[])
{
    DummyApplication app(argc, argv);
    DummyApplicationAdaptor adaptor(&app);

    // Can not register object at /
    QDBusConnection::sessionBus().registerService("com.onyx.service.dummy");
    QDBusConnection::sessionBus().registerObject("/com/onyx/object/dummy",
                                                 &app);
    return app.exec();
}
