#ifndef DUMMY_APPLICATION_H_
#define DUMMY_APPLICATION_H_

#include "onyx/base/base.h"
#include "onyx/base/ui.h"
#include "onyx/base/dbus.h"
#include "dummy_view.h"

using namespace ui;

namespace dummy
{

/// Dummy Application.
class DummyApplication : public Application
{
    Q_OBJECT
public:
    DummyApplication(int &argc, char **argv);
    ~DummyApplication(void);

public slots:
    bool Open(const QString &path_name);
    bool Close(const QString &path_name);

private:

    QDBusConnection connection_;
    DummyView view_;

    NO_COPY_AND_ASSIGN(DummyApplication);
};


class DummyApplicationAdaptor : public QDBusAbstractAdaptor
{
    Q_OBJECT;

    Q_CLASSINFO("D-Bus Interface", "com.onyx.interface.dummy");

public:
    DummyApplicationAdaptor(DummyApplication *application)
        : QDBusAbstractAdaptor(application)
        , app_(application)
    {
    }

public slots:
    /// Must be in dbus data type, otherwise these methods will not
    /// be exported as dbus methods. So you can not use std::string
    /// here.
    bool Open(const QString & path) {return app_->Open(path);}
    bool Close(const QString & path) {return app_->Close(path);}

private:
    DummyApplication *app_;
    NO_COPY_AND_ASSIGN(DummyApplicationAdaptor);

};  // DummyApplicationAdaptor

}; // namespace dummy

#endif
