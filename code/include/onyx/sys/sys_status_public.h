#ifndef SYS_STATUS_PUBLIC_H_
#define SYS_STATUS_PUBLIC_H_

namespace sys
{


/// SysStatus serves as a proxy between system manager and caller.
/// Caller uses SysStatus to get system status and to change system status.
class SysStatus : public QObject
{
    Q_OBJECT

public:
    static SysStatus & instance()
    {
        static SysStatus instance_;
        return instance_;
    }
    ~SysStatus();

public Q_SLOTS:
    bool batteryStatus(int& left, int& status);

    void rotateScreen();
    bool setScreenTransformation(int degree);
    int  screenTransformation();

    void setSystemBusy(bool busy = true, bool show_indicator = true);
    inline bool isSystemBusy() { return system_busy_; }

    void snapshot(const QString &path);

public:
    void dbgUpdateBattery(int left, int status);

public:
    void dump();

Q_SIGNALS:

    /// connectToPC is emitted when the device is connected to PC.
    /// This signal is not emitted when user connects usb charger
    /// to device. This signal does not mean the device works in usb
    /// slave mode.
    void connectToPC(bool connected);

    void volumeChanged(int new_volume, bool is_mute);

    void aboutToSuspend();
    void wakeup();
    void aboutToShutdown();

    void stylusChanged(bool inserted);

private:
    SysStatus();
    SysStatus(SysStatus & ref);

};

};  // namespace sys

#endif  // SYS_STATUS_PUBLIC_H_
