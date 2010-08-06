
#include "onyx/sys/messenger_proxy.h"

static const QString service = "com.onyx.service.system_manager";
static const QString object  = "/com/onyx/object/system_manager";
static const QString iface   = "com.onyx.interface.system_manager";


using namespace content;

MessengerProxy::MessengerProxy()
#ifndef _WINDOWS
    : connection_(QDBusConnection::systemBus())
#else
    : connection_(QDBusConnection::sessionBus())
#endif
{
}

MessengerProxy::~MessengerProxy()
{
}

bool MessengerProxy::checkNewContent(bool check_all)
{
    return 0;
}

bool MessengerProxy::confirmNewContent(const QStringList ids)
{
    return false;
}

bool MessengerProxy::updateAdobeId(QString & id)
{
    return true;
}

void MessengerProxy::receiveMessages()
{
}

void MessengerProxy::onAddressAcquired(bool)
{
}
