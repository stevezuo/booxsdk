// Authors: John

/// Public header of the system configuration library.

#ifndef SYS_SERVICE_CONF_H__
#define SYS_SERVICE_CONF_H__

#include "onyx/base/base.h"
#include "onyx/ui/ui_global.h"
#include <QtSql/QtSql>

using namespace base;

namespace sys
{

/// Maintains all metadata of a service. A service usually enables
/// user to view a document or configure the system.
/// When the service uses dbus, the service name must not be empty.
/// It's not necessary for the application to use dbus always.
/// When it does not use the dbus, the service name can be empty.
/// No matter it uses the dbus, the app name can never be empty.
class Service
{
public:
    Service();
    Service(const QString &svr, const QString &obj,
            const QString &ifn, const QString &m, const QString &app);
    Service(bool runnable);
    Service(const Service &right);
    ~Service();

    Service & operator = (const Service & right);
    bool operator == (const Service & right);

    const QString & service_name() const { return service_name_; }
    const QString & object_path() const { return object_path_; }
    const QString & interface_name() const { return interface_name_; }
    const QString & method() const { return method_; }

    const QString & app_name() const { return app_name_; }
    QString & mutable_app_name() { return app_name_; }

    const QStringList & extensions() const { return extension_name_; }
    QStringList & mutable_extensions() { return extension_name_; }

    bool runnable() { return runnable_; }

private:
    QStringList  extension_name_;
    QString      service_name_;      ///< Service name.
    QString      object_path_;       ///< Executable file name.
    QString      interface_name_;
    QString      method_;
    QString      app_name_;          ///< The exectuable file name.
    bool runnable_;
};
typedef Service * ServicePtr;
typedef list<Service> Services;
typedef Services::iterator ServicesIter;
typedef Services::const_iterator ConstServicesIter;

class ServiceConfig
{
    friend class SystemConfig;
public:
    ServiceConfig();
    ~ServiceConfig();

private:
    static bool makeSureTableExist(QSqlDatabase& database);

    static void loadAllServices(QSqlDatabase &, Services &);
    static bool calibrationService(QSqlDatabase &, Service &);
    static bool metService(QSqlDatabase &, Service &);
    static bool musicService(QSqlDatabase &, Service & service);
    static bool noteService(QSqlDatabase &, Service & service);
    static bool networkService(QSqlDatabase &, Service &);
    static bool webBrowserService(QSqlDatabase &, Service &);
    static bool DRMService(QSqlDatabase &, Service &service);
    static bool messengerService(QSqlDatabase &, Service &service);
    static bool writePadService(QSqlDatabase &, Service & service);
    static bool dictionaryService(QSqlDatabase &, Service & service);
    static bool rssService(QSqlDatabase&, Service &);
    static bool sudokuService(QSqlDatabase&, Service &);
    static bool officeViewerService(QSqlDatabase &, Service & service);
    static bool hasOfficeViewer();
    static bool nabooReaderService(QSqlDatabase &, Service & service);
    static bool onyxReaderService(QSqlDatabase &, Service & service);
    static bool comicReaderService(QSqlDatabase &, Service & service);

    static bool checkService(QSqlDatabase &, const Service &);
    static bool addService(QSqlDatabase &, const Service &);
    static bool registerService(QSqlDatabase&, const Service&, const QString&);
    static bool unRegisterService(QSqlDatabase &, const Service &);

    static void loadDefaultServices();
};

};

#endif  // SYS_SERVICE_CONF_H__
