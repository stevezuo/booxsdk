#ifndef MESSENGER_PROXY_H_
#define MESSENGER_PROXY_H_

#include <string>
#include <QtCore/QtCore>
#include "onyx/base/base.h"
#include "onyx/base/dbus.h"
#include "onyx/data/content.h"

using namespace content;

/// Maintains connection to messenger daemon.
/// It sends command to messenger and broadcasts
/// messages to all listeners.
class MessengerProxy : public QObject
{
    Q_OBJECT

public:
    MessengerProxy();
    ~MessengerProxy();

public Q_SLOTS:
    bool checkNewContent(bool check_all = false);
    bool confirmNewContent(const QStringList ids);
    bool updateAdobeId(QString & id);

Q_SIGNALS:

private Q_SLOTS:
    void receiveMessages();
    void onAddressAcquired(bool);

private:
    QDBusConnection connection_;    ///< Connection to wpa_supplicant.

};

#endif      // MESSENGER_PROXY_H_
