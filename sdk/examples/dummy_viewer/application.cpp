
#include "application.h"

namespace dummy
{

DummyApplication::DummyApplication(int &argc, char **argv)
    : Application(argc, argv)
    , connection_(QDBusConnection::sessionBus())
    , view_(0)
{
    view_.show();
}

DummyApplication::~DummyApplication(void)
{
}

bool DummyApplication::Open(const QString &path)
{
    view_.SetPath(path);
    return true;
}

bool DummyApplication::Close(const QString &path)
{
    view_.SetPath("");
    return true;
}


}
// namespace dummy
