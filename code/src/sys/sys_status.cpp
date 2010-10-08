
#include "onyx/base/device.h"
#include <QtGui/QtGui>
#ifdef BUILD_FOR_ARM
#include <QtGui/qwsdisplay_qws.h>
#include <QtGui/qscreen_qws.h>
#endif
#include <QtNetwork/QtNetwork>
#include "onyx/sys/sys_status.h"
#include "onyx/sys/sys_conf.h"

namespace sys
{

static const QString service = "com.onyx.service.system_manager";
static const QString object  = "/com/onyx/object/system_manager";
static const QString iface   = "com.onyx.interface.system_manager";

static const QString MOBILE_DEVICE = "mobile";
static const QString TETHERED_DEVICE = "tethered";
static const QString DEVICE_NAME = "OnyxBoox";
static const QString ROOT_FOLDER = "/";
static const QString DRM_CONTENT_DIR = "media/flash/DRM Contents";
static const QString DEVICE_FILE     = "media/flash/.adobe-digital-editions/device.xml";
static const QString ACTIVATION_FILE = "media/flash/.adobe-digital-editions/activation.xml";

static void putDeviceType()
{
    QString is_mobile;
    is_mobile.setNum(1);
    qputenv("ADOBE_DE_MOBILE", is_mobile.toAscii());

    QString device_type = SysStatus::instance().hasTouchScreen() ? MOBILE_DEVICE : TETHERED_DEVICE;
    qputenv("ADEPT_DEVICE_TYPE", device_type.toAscii());

    qDebug("ADOBE_DE_MOBILE : %s", qgetenv("ADOBE_DE_MOBILE").constData());
    qDebug("ADEPT_DEVICE_TYPE : %s", qgetenv("ADEPT_DEVICE_TYPE").constData());
}

static void putRootFolder()
{
    qputenv("ADOBE_DE_ROOT_FOLDER", ROOT_FOLDER.toAscii());
    qDebug("ADOBE_DE_ROOT_FOLDER : %s", qgetenv("ADOBE_DE_ROOT_FOLDER").constData());
}

static void putDocumentFolder()
{
    // set the folder of Digital Edition
    QString path;
#ifdef Q_WS_QWS
    path = ROOT_FOLDER;
#else
    path = QDir::home().path();
#endif
    QDir dir(path);
    qputenv("ADOBE_DE_DOC_FOLDER", dir.absoluteFilePath(DRM_CONTENT_DIR).toAscii());
    qDebug("ADOBE_DE_DOC_FOLDER : %s", qgetenv("ADOBE_DE_DOC_FOLDER").constData());
}

static void putDeviceName()
{
    if (qgetenv("ADEPT_DEVICE_NAME").isEmpty())
    {
        qputenv("ADEPT_DEVICE_NAME", DEVICE_NAME.toAscii());
    }
    qDebug("ADEPT_DEVICE_NAME : %s", qgetenv("ADEPT_DEVICE_NAME").constData());
}

static void putDeviceFile()
{
    // set the folder of Digital Edition
    QString path;
#ifdef Q_WS_QWS
    path = ROOT_FOLDER;
#else
    path = QDir::home().path();
#endif
    QDir dir(path);
    qputenv("ADEPT_DEVICE_FILE", dir.absoluteFilePath(DEVICE_FILE).toAscii());
    qDebug("ADEPT_DEVICE_FILE : %s", qgetenv("ADEPT_DEVICE_FILE").constData());

    qputenv("ADEPT_ACTIVATION_FILE", dir.absoluteFilePath(ACTIVATION_FILE).toAscii());
    qDebug("ADEPT_ACTIVATION_FILE : %s", qgetenv("ADEPT_ACTIVATION_FILE").constData());
}

SysStatus::SysStatus()
#ifndef _WINDOWS
    : connection_(QDBusConnection::systemBus())
#else
    : connection_(QDBusConnection::sessionBus())
#endif
    , usb_mounted_(false)
    , sd_mounted_(false)
    , flash_mounted_(false)
    , system_busy_(true)
{
    installSlots();
}

SysStatus::SysStatus(SysStatus & ref)
#ifndef _WINDOWS
: connection_(QDBusConnection::systemBus())
#else
: connection_(QDBusConnection::sessionBus())
#endif
, usb_mounted_(false)
, sd_mounted_(false)
, system_busy_(true)
{
}

SysStatus::~SysStatus()
{
}

/// Register slots to system manager to enable application to receive signals from
/// system manager.
void SysStatus::installSlots()
{
    if (!connection_.connect(service, object, iface,
                             "mountTreeSignal",
                             this,
                             SLOT(onMountTreeChanged(bool, const QString &))))
    {
        qDebug("\nCan not connect the mountTreeSignal!\n");
    }

    if (!connection_.connect(service, object, iface,
                             "sdCardChangedSignal",
                             this,
                             SLOT(onSdCardChanged(bool))))
    {
        qDebug("\nCan not connect the sdCardChangedSignal!\n");
    }

    if (!connection_.connect(service, object, iface,
                             "sdioChangedSignal",
                             this,
                             SLOT(onSdioChanged(bool))))
    {
        qDebug("\nCan not connect the sdioChangedSignal!\n");
    }

    if (!connection_.connect(service, object, iface,
                             "batterySignal",
                             this,
                             SLOT(onBatteryChanged(int,int))))
    {
        qDebug("\nCan not connect the batterySignal\n");
    }

    if (!connection_.connect(service, object, iface,
                             "connectToPC",
                             this,
                             SLOT(onConnectToPC(bool))))
    {
        qDebug("\nCan not connect the connectToPC\n");
    }

    if (!connection_.connect(service, object, iface,
                             "inUSBSlaveMode",
                             this,
                             SLOT(onInUSBSlaveMode())))
    {
        qDebug("\nCan not connect the inUSBSlaveMode\n");
    }

    if (!connection_.connect(service, object, iface,
                             "screenRotated",
                             this,
                             SLOT(onScreenRotated(const int))))
    {
        qDebug("\nCan not connect the screenRotated\n");
    }

    if (!connection_.connect(service, object, iface,
                             "aboutToSuspend",
                             this,
                             SLOT(onAboutToSuspend())))
    {
        qDebug("\nCan not connect the aboutToSuspend\n");
    }

    if (!connection_.connect(service, object, iface,
                             "wakeup",
                             this,
                             SLOT(onWakeup())))
    {
        qDebug("\nCan not connect the wakeup\n");
    }

    if (!connection_.connect(service, object, iface,
                             "aboutToShutdown",
                             this,
                             SLOT(onAboutToShutdown())))
    {
        qDebug("\nCan not connect the aboutToShutdown\n");
    }

    if (!connection_.connect(service, object, iface,
                             "forceQuit",
                             this,
                             SLOT(onForceQuit())))
    {
        qDebug("\nCan not connect the forceQuit\n");
    }

    if (!connection_.connect(service, object, iface,
                             "onlineServiceSignal",
                             this,
                             SLOT(onlineService())))
    {
        qDebug("\nCan not connect the onlineService\n");
    }

    if (!connection_.connect(service, object, iface,
                             "pppConnectionChanged",
                             this,
                             SLOT(onPppConnectionChanged(const QString&, int))))
    {
        qDebug("\nCan not connect the pppConnectionChanged\n");
    }

    if (!connection_.connect(service, object, iface,
                             "volumeChanged",
                             this,
                             SLOT(onVolumeChanged(int, bool))))
    {
        qDebug("\nCan not connect the volume changed\n");
    }

    if (!connection_.connect(service, object, iface,
                             "stylusChanged",
                             this,
                             SLOT(onStylusChanged(bool))))
    {
        qDebug("\nCan not connect the signal stylus changed\n");
    }

    if (!connection_.connect(service, object, iface,
                             "musicPlayerStateChanged",
                             this,
                             SLOT(onMusicPlayerStateChanged(int))))
    {
        qDebug("\nCan not connect the musicPlayerStateChanged signal\n");
    }

    if (!connection_.connect(service, object, iface,
                             "downloadStateChanged",
                             this,
                             SLOT(onDownloadStateChanged(const QString &, int, bool))))
    {
        qDebug("\nCan not connect the downloadStateChanged signal\n");
    }

    if (!connection_.connect(service, object, iface,
                             "requestDRMUserInfo",
                             this,
                             SLOT(onRequestDRMUserInfo(const QString &, const QString &))))
    {
        qDebug("\nCan not connect the requestDRMUserInfo signal\n");
    }

    if (!connection_.connect(service, object, iface,
                             "fulfillmentFinished",
                             this,
                             SLOT(onFulfillmentFinished(const QString &))))
    {
        qDebug("\nCan not connect the fulfillmentFinished signal\n");
    }

    if (!connection_.connect(service, object, iface,
                             "loanReturnFinished",
                             this,
                             SLOT(onLoanReturnFinished(const QString &))))
    {
        qDebug("\nCan not connect the loanReturnFinished signal\n");
    }

    if (!connection_.connect(service, object, iface,
                             "reportWorkflowError",
                             this,
                             SLOT(onReportWorkflowError(const QString &, const QString &))))
    {
        qDebug("\nCan not connect the reportWorkflowError signal\n");
    }

    if (!connection_.connect(service, object, iface,
                             "signalStrengthChanged",
                             this,
                             SLOT(onReport3GNetwork(const int, const int, const int))))
    {
        qDebug("\nCan not connect the signalStrengthChanged signal\n");
    }

    
}

namespace {
bool checkAndReturnBool(const QList<QVariant> & args)
{
    if (args.size() > 0)
    {
        return args.at(0).toBool();
    }
    return false;
}
}  // namespace

bool SysStatus::batteryStatus(int& current,
                              int& status)
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "batteryStatus"      // method.
    );

    // Call.
    QDBusMessage reply = connection_.call(message);

    if (reply.type() == QDBusMessage::ReplyMessage)
    {
        QList<QVariant> args = reply.arguments();
        current = args[1].toInt();
        status = args[2].toInt();
        return true;
    }
    else if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
    }
    return false;
}

/// Query whether USB is mounted or not.
/// TODO: improve it
bool SysStatus::isUSBMounted()
{
    static bool queried = false;
    if (!queried)
    {
        QFile file(USB_DEVICE);
        usb_mounted_ = file.open(QIODevice::ReadOnly);
        queried = true;
    }
    return usb_mounted_;
}

bool SysStatus::isSDMounted()
{
    return QFile::exists(SDMMC_DEVICE);
}

bool SysStatus::isFlashMounted()
{
#ifdef _WINDOWS
    return true;
#endif

    QFile file("/proc/mounts");
    if (!file.open(QIODevice::ReadOnly))
    {
        return false;
    }
    return file.readAll().contains(LIBRARY_ROOT);
}


bool SysStatus::umountUSB()
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "umountUSB"      // method.
    );

    QDBusMessage reply = connection_.call(message);

    if (reply.type() == QDBusMessage::ReplyMessage)
    {
        return checkAndReturnBool(reply.arguments());
    }
    else if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
    }
    return false;
}

bool SysStatus::umountSD()
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "umountSD"      // method.
    );

    QDBusMessage reply = connection_.call(message);

    if (reply.type() == QDBusMessage::ReplyMessage)
    {
        return checkAndReturnBool(reply.arguments());
    }
    else if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
    }
    return false;
}

/// Ask the device to work in USB slave mode.
/// Usually this function is called when user connects device to PC
/// through a USB cable.
bool SysStatus::workInUSBSlaveMode()
{
    // Emit another signal to ensure caller close all resource.
    emit inUSBSlaveMode();

    // Now tell system manager to
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "workInUSBSlaveMode"      // method.
    );

    QDBusMessage reply = connection_.call(message);

    if (reply.type() == QDBusMessage::ReplyMessage)
    {
        return checkAndReturnBool(reply.arguments());
    }
    else if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
    }
    return false;
}


bool SysStatus::enableSdio(bool enable) const
{
#ifdef WIN32
    return true;
#endif
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "enableSdio"      // method.
    );

    message << enable;
    QDBusMessage reply = connection_.call(message);

    if (reply.type() == QDBusMessage::ReplyMessage)
    {
        return checkAndReturnBool(reply.arguments());
    }
    else if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
    }
    return false;
}

bool SysStatus::sdioState() const
{
#ifdef WIN32
    return true;
#endif

    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "sdioState"      // method.
    );

    QDBusMessage reply = connection_.call(message);

    if (reply.type() == QDBusMessage::ReplyMessage)
    {
        return checkAndReturnBool(reply.arguments());
    }
    else if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
    }
    return false;
}

bool SysStatus::enableSdio(bool enable)
{
#ifdef WIN32
    return true;
#endif
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "enableSdio"      // method.
    );

    message << enable;
    QDBusMessage reply = connection_.call(message);

    if (reply.type() == QDBusMessage::ReplyMessage)
    {
        return checkAndReturnBool(reply.arguments());
    }
    else if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
    }
    return false;
}

void SysStatus::rotateScreen()
{
    int degree = screenTransformation();
    if (degree == 0)
    {
        setScreenTransformation(90);
    }
    else if (degree == 90)
    {
        setScreenTransformation(270);
    }
    else if (degree == 270)
    {
        setScreenTransformation(0);
    }
}

bool SysStatus::setScreenTransformation(int degree)
{
#ifdef BUILD_FOR_ARM
    QWSDisplay::setTransformation(degree / 90);

    // Also write to system conf file.
    const QString PATH = "/root/Settings/screen";
    QFile file(PATH);
    if (!file.open(QIODevice::WriteOnly|QIODevice::Truncate))
    {
        qWarning("Could not open %s for writing", qPrintable(PATH));
    }
    else
    {
        QString data("export QWS_DISPLAY=Transformed:Rot%1:OnyxScreen:/dev/mem");
        data = data.arg(degree);
        file.write(data.toAscii());
        file.flush();
        file.close();
    }
#endif
    return true;
}

int  SysStatus::screenTransformation()
{
    int degree = 0;
#ifdef BUILD_FOR_ARM
    degree = QScreen::instance()->transformOrientation() * 90;
#endif
    return degree;
}

/// The calibration must be done in the server process.
#ifdef BUILD_FOR_ARM
bool SysStatus::clearCalibration()
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "clearCalibration"      // method.
    );

    QDBusMessage reply = connection_.call(message);

    if (reply.type() == QDBusMessage::ReplyMessage)
    {
        return checkAndReturnBool(reply.arguments());
    }
    else if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
    }
    return false;
}

bool SysStatus::calibrate(QWSPointerCalibrationData & data)
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "calibrate"      // method.
    );

    for(int i = static_cast<int>(QWSPointerCalibrationData::TopLeft);
        i <= static_cast<int>(QWSPointerCalibrationData::Center);
        ++i)
    {
        message << data.screenPoints[i].x();
        message << data.screenPoints[i].y();
    }

    for(int i = static_cast<int>(QWSPointerCalibrationData::TopLeft);
        i <= static_cast<int>(QWSPointerCalibrationData::Center);
        ++i)
    {
        message << data.devPoints[i].x();
        message << data.devPoints[i].y();
    }

    QDBusMessage reply = connection_.call(message);

    if (reply.type() == QDBusMessage::ReplyMessage)
    {
        return checkAndReturnBool(reply.arguments());
    }
    else if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
    }
    return false;
}
#endif

bool SysStatus::setSuspendInterval(int ms)
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "setSuspendInterval"      // method.
    );
    message << ms;

    QDBusMessage reply = connection_.call(message);

    if (reply.type() == QDBusMessage::ReplyMessage)
    {
        return checkAndReturnBool(reply.arguments());
    }
    else if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
    }
    return false;
}

int  SysStatus::suspendInterval()
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "suspendInterval"      // method.
    );

    QDBusMessage reply = connection_.call(message);

    if (reply.type() == QDBusMessage::ReplyMessage)
    {
        QList<QVariant> args = reply.arguments();
        return args[0].toInt();
    }
    else if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
    }
    return 0;
}

bool SysStatus::suspend()
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "suspend"      // method.
    );

    QDBusMessage reply = connection_.call(message);

    if (reply.type() == QDBusMessage::ReplyMessage)
    {
        return checkAndReturnBool(reply.arguments());
    }
    else if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
    }
    return true;
}

bool SysStatus::setShutdownInterval(int ms)
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "setShutdownInterval"      // method.
    );
    message << ms;

    QDBusMessage reply = connection_.call(message);

    if (reply.type() == QDBusMessage::ReplyMessage)
    {
        return checkAndReturnBool(reply.arguments());
    }
    else if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
    }
    return false;
}

int  SysStatus::shutdownInterval()
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "shutdownInterval"      // method.
    );

    QDBusMessage reply = connection_.call(message);

    if (reply.type() == QDBusMessage::ReplyMessage)
    {
        QList<QVariant> args = reply.arguments();
        return args[0].toInt();
    }
    else if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
    }
    return 0;
}

void SysStatus::shutdown(int reason)
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "shutdown"      // method.
    );

    message << reason;
    connection_.asyncCall(message);
}

void SysStatus::resetIdle()
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "resetIdle"      // method.
    );
    connection_.call(message);
}

void SysStatus::enableIdle(bool enable)
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "enableIdle"      // method.
    );

    message << enable;
    connection_.call(message);
}

bool SysStatus::isIdleEnabled()
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "isIdleEnabled"      // method.
    );

    QDBusMessage reply = connection_.call(message);

    if (reply.type() == QDBusMessage::ReplyMessage)
    {
        return checkAndReturnBool(reply.arguments());
    }
    else if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
    }
    return false;
}

/// Show or hide music player. Usually, this function is used by
/// viewer or normal application to launch music player.
void SysStatus::requestMusicPlayer(int cmd)
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "requestMusicPlayer"      // method.
    );

    message << cmd;
    connection_.asyncCall(message);
}

/// Broadcast music player state signal. This function is used
/// by music player to notify all listeners.
void SysStatus::reportMusicPlayerState(int state)
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "reportMusicPlayerState"      // method.
    );

    message << state;
    connection_.asyncCall(message);
}

int SysStatus::volume()
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "volume"      // method.
    );

    QDBusMessage reply = connection_.call(message);

    if (reply.type() == QDBusMessage::ReplyMessage)
    {
        QList<QVariant> args = reply.arguments();
        return args[0].toInt();
    }
    else if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
    }
    return -1;
}

bool SysStatus::setVolume(int volume)
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "setVolume"      // method.
    );
    message << volume;

    QDBusMessage reply = connection_.call(message);

    if (reply.type() == QDBusMessage::ReplyMessage)
    {
        return checkAndReturnBool(reply.arguments());
    }
    else if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
    }
    return false;
}

bool SysStatus::mute(bool m)
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "mute"      // method.
    );
    message << m;

    QDBusMessage reply = connection_.call(message);

    if (reply.type() == QDBusMessage::ReplyMessage)
    {
        return checkAndReturnBool(reply.arguments());
    }
    else if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
    }
    return false;
}

bool SysStatus::isMute()
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "isMute"      // method.
    );

    QDBusMessage reply = connection_.call(message);

    if (reply.type() == QDBusMessage::ReplyMessage)
    {
        return checkAndReturnBool(reply.arguments());
    }
    else if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
    }
    return false;
}

bool SysStatus::isWpaSupplicantRunning()
{
#ifdef WIN32
    return true;
#endif

    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "isWpaSupplicantRunning"      // method.
    );

    QDBusMessage reply = connection_.call(message);
    if (reply.type() == QDBusMessage::ReplyMessage)
    {
        return checkAndReturnBool(reply.arguments());
    }
    else if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
    }
    return false;
}

bool SysStatus::startWpaSupplicant(const QString & conf_file_path)
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "startWpaSupplicant"      // method.
    );
    message << conf_file_path;

    QDBusMessage reply = connection_.call(message);
    if (reply.type() == QDBusMessage::ReplyMessage)
    {
        return checkAndReturnBool(reply.arguments());
    }
    else if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
    }
    return false;
}

bool SysStatus::stopWpaSupplicant()
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "stopWpaSupplicant"      // method.
    );

    QDBusMessage reply = connection_.call(message);
    if (reply.type() == QDBusMessage::ReplyMessage)
    {
        return checkAndReturnBool(reply.arguments());;
    }
    else if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
    }
    return false;
}

bool SysStatus::connect3g(const QString & chat_file,
                          const QString & username,
                          const QString & password)
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "connect3g"      // method.
    );

    message << chat_file;
    message << username;
    message << password;

    QDBusMessage reply = connection_.call(message);
    if (reply.type() == QDBusMessage::ReplyMessage)
    {
        return checkAndReturnBool(reply.arguments());;
    }
    else if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
    }
    return false;
}

void SysStatus::disconnect3g()
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "disconnect3g"      // method.
    );

    QDBusMessage reply = connection_.call(message);
    if (reply.type() == QDBusMessage::ReplyMessage)
    {
        return;
    }
    else if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
    }
}

bool SysStatus::isPowerSwitchOn()
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "isPowerSwitchOn"      // method.
    );

    QDBusMessage reply = connection_.call(message);
    if (reply.type() == QDBusMessage::ReplyMessage)
    {
        return checkAndReturnBool(reply.arguments());;
    }
    else if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
    }
    return false;
}


QString SysStatus::currentConnection()
{
    QString result;
    QList<QNetworkInterface> all = QNetworkInterface::allInterfaces();
    foreach(QNetworkInterface ni, all)
    {
        if (ni.name().contains("eth", Qt::CaseInsensitive))
        {
            return "wifi";
        }
        else if (ni.name().contains("ppp", Qt::CaseInsensitive))
        {
            return "3g";
        }
    }
    return result;
}

/// Check hardware connection type. It returns wifi or 3g.
/// But it does not mean the connection has been established.
QString SysStatus::connectionType()
{
    if (qgetenv("TG").toInt() > 0)
    {
        return "3g";
    }
    return "wifi";
}

/// Retrieve the wpa_proxy instance. Before using this function make sure
/// you call startWpaSupplicant already. Although dbus supports activation,
/// but we need to start wpa_supplicant with different conf file sometimes,
/// so it's better to ask system manager to start wpa_supplicant with different
/// parameters.
WpaConnection & SysStatus::wpa_proxy(const QString & name)
{
    if (!wpa_proxy_)
    {
        wpa_proxy_.reset(new WpaConnection(name));
    }
    return *wpa_proxy_;
}

void SysStatus::setSystemBusy(bool busy,
                              bool show_indicator)
{
    system_busy_ = busy;
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "setSystemBusy"      // method.
    );
    message << busy;
    message << show_indicator;
    connection_.call(message);
}

void SysStatus::reportDownloadState(const QString &path,
                                    int percentage,
                                    bool open)
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "reportDownloadState"      // method.
    );
    message << path;
    message << percentage;
    message << open;
    QDBusMessage reply = connection_.call(message);
    if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
    }
}

void SysStatus::triggerOnlineService()
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "triggerOnlineService"      // method.
    );
    connection_.call(message);
}

bool SysStatus::setGrayScale(int colors)
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "setGrayScale"      // method.
    );
    message << colors;
    QDBusMessage reply = connection_.call(message);
    if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
        return false;
    }
    return true;
}

int SysStatus::grayScale()
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "grayScale"      // method.
    );

    QDBusMessage reply = connection_.call(message);
    if (reply.type() == QDBusMessage::ReplyMessage)
    {
        if (reply.arguments().size() > 0)
        {
            return reply.arguments().front().toInt();
        }
    }
    else if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
    }
    return 8;
}

bool SysStatus::startDRMService(const QStringList &strings)
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "startDRMService"      // method.
    );
    message << strings;

    QDBusMessage reply = connection_.call(message);
    if (reply.type() == QDBusMessage::ReplyMessage)
    {
        if (reply.arguments().size() > 0)
        {
            return reply.arguments().front().toBool();
        }
    }
    else if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
    }
    return false;
}

bool SysStatus::stopDRMService()
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "stopDRMService"      // method.
    );

    QDBusMessage reply = connection_.call(message);
    if (reply.type() == QDBusMessage::ReplyMessage)
    {
        if (reply.arguments().size() > 0)
        {
            return reply.arguments().front().toBool();
        }
    }
    else if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
    }
    return false;
}

bool SysStatus::startMessenger()
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "startMessenger"      // method.
    );

    QDBusMessage reply = connection_.call(message);
    if (reply.type() == QDBusMessage::ReplyMessage)
    {
        if (reply.arguments().size() > 0)
        {
            return reply.arguments().front().toBool();
        }
    }
    else if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
    }
    return false;
}

bool SysStatus::stopMessenger()
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "stopMessenger"      // method.
    );

    QDBusMessage reply = connection_.call(message);
    if (reply.type() == QDBusMessage::ReplyMessage)
    {
        if (reply.arguments().size() > 0)
        {
            return reply.arguments().front().toBool();
        }
    }
    else if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
    }
    return false;
}

void SysStatus::addDRMEnvironment()
{
    SystemConfig conf;
    qputenv("DEVICE_SERIAL", conf.serialNumber().toAscii());

    putDeviceType();
    putRootFolder();
    putDocumentFolder();
    putDeviceName();
    putDeviceFile();
}

// TODO, implement in system manager later.
bool SysStatus::setAsScreensaver(const QString & path)
{
    QDir dir(SHARE_ROOT);
    dir.cd("system_manager/images");

    int count =  dir.entryInfoList(QDir::NoDotAndDotDot|QDir::Files).size();
    QString result_path("screen_saver%1.png");
    result_path = result_path.arg(count);
    result_path = dir.absoluteFilePath(result_path);
    return QFile::copy(path, result_path);
}

void SysStatus::snapshot(const QString &path)
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "snapshot"      // method.
    );
    message << path;
    QDBusMessage reply = connection_.call(message);
    if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
    }
}

bool SysStatus::hasTouchScreen()
{
#ifdef _WINDOWS
    return true;
#endif

    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "hasTouchScreen"      // method.
    );

    QDBusMessage reply = connection_.call(message);
    if (reply.type() == QDBusMessage::ReplyMessage)
    {
        if (reply.arguments().size() > 0)
        {
            return reply.arguments().front().toBool();
        }
    }
    else if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
    }
    return true;
}

void SysStatus::dbgUpdateBattery(int left, int status)
{
    QDBusMessage message = QDBusMessage::createMethodCall(
        service,            // destination
        object,             // path
        iface,              // interface
        "dbgUpdateBattery"      // method.
    );
    message << left;
    message << status;
    QDBusMessage reply = connection_.call(message);
    if (reply.type() == QDBusMessage::ErrorMessage)
    {
        qWarning("%s", qPrintable(reply.errorMessage()));
    }
}

void SysStatus::dump()
{
    int left;
    int status;
    batteryStatus(left, status);
    qDebug("Bettery %d %d", left, status);
}

/// Handle mount tree signal.
void SysStatus::onMountTreeChanged(bool inserted, const QString &mount_point)
{
    if (mount_point == SDMMC_ROOT)
    {
        sd_mounted_ = inserted;
    }
    if (mount_point == USB_ROOT)
    {
        usb_mounted_ = inserted;
    }
    if (mount_point == LIBRARY_ROOT)
    {
        flash_mounted_ = inserted;
    }
    emit mountTreeSignal(inserted, mount_point);
}

/// The SD card hardware event handler. It does not mean the mount tree changed.
void SysStatus::onSdCardChanged(bool insert)
{
    emit sdCardChangedSignal(insert);
}

void SysStatus::onSdioChanged(bool on)
{
    emit sdioChangedSignal(on);
}

void SysStatus::onBatteryChanged(int current,
                                 int status)
{
    emit batterySignal(current, status);
}

void SysStatus::onConnectToPC(bool connected)
{
    emit connectToPC(connected);
}

void SysStatus::onInUSBSlaveMode()
{
    emit inUSBSlaveMode();
}

void SysStatus::onSystemIdle()
{
    emit systemIdleSignal();
}

void SysStatus::onScreenRotated(const int degree)
{
    emit screenRotated(degree);
}

void SysStatus::onAboutToSuspend()
{
    emit aboutToSuspend();
}

void SysStatus::onWakeup()
{
    emit wakeup();
}

void SysStatus::onAboutToShutdown()
{
    emit aboutToShutdown();
}

void SysStatus::onPppConnectionChanged(const QString &message, int state)
{
    emit pppConnectionChanged(message, state);
}

void SysStatus::onlineService()
{
    emit onlineServiceRequest();
}

void SysStatus::onForceQuit()
{
    emit forceQuit();
}

void SysStatus::onStylusChanged(bool inserted)
{
    emit stylusChanged(inserted);
}

void SysStatus::onMusicPlayerStateChanged(int state)
{
    emit musicPlayerStateChanged(state);
}

void SysStatus::onDownloadStateChanged(const QString &url,
                                       int state,
                                       bool open)
{
    emit downloadStateChanged(url, state, open);
}

void SysStatus::onVolumeChanged(int new_volume, bool is_mute)
{
    emit volumeChanged(new_volume, is_mute);
}

void SysStatus::onRequestDRMUserInfo(const QString &string, const QString & param)
{
    emit requestDRMUserInfo(string, param);
}

void SysStatus::onFulfillmentFinished(const QString & string)
{
    emit fulfillmentFinished(string);
}

void SysStatus::onLoanReturnFinished(const QString & string)
{
    emit loanReturnFinished(string);
}

void SysStatus::onReportWorkflowError(const QString & workflow, const QString & error_code)
{
    emit reportWorkflowError(workflow, error_code);
}

void SysStatus::onReport3GNetwork(const int signal, const int total, const int network)
{
    emit report3GNetwork(signal, total, network);
}
}   // namespace sys
