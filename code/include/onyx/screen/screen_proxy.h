// -*- mode: c++; c-basic-offset: 4; -*-

#ifndef SCREEN_PROXY_H_
#define SCREEN_PROXY_H_

#include <QtNetwork/QtNetwork>

#include "onyx/base/base.h"

namespace onyx
{
namespace screen
{

struct ScreenCommand
{
    enum Type {
        WAIT_FOR_FINISHED = 0,
        SYNC,
        UPDATE,
        SYNC_AND_UPDATE,
        DRAW_LINE,
        DRAW_LINES,
        FILL_SCREEN,
    };

    enum WaitMode {
        WAIT_NONE           = 0,  ///< Don't wait.
        WAIT_BEFORE_UPDATE  = 0x01,  ///< Ensure previous update is finished.
        WAIT_COMMAND_FINISH = 0x02,  ///< Ensure command has been processed.
        WAIT_NORMAL         = WAIT_BEFORE_UPDATE,
        WAIT_ALL            = WAIT_BEFORE_UPDATE | WAIT_COMMAND_FINISH
    };

    enum UpdateMode {
        FULL_UPDATE = 0,
        PARTIAL_UPDATE
    };

    static const int MAX_POINTS = 15;
    Type type;
    int waveform;
    UpdateMode update_flags;               ///< Partial update or full screen update.
    int top, left, width, height;   ///< The region to update.
    QPoint points[MAX_POINTS];
    int point_count;
    int color;
    int size;
    WaitMode wait_flags;                 ///< Wait flags.
};

extern const int PORT;

/// Screen proxy is a proxy used to talk with the screen manager daemon.
class ScreenProxy
{
public:
    ///
    /// Define the waveform to update screen.
    ///
    /// - Direct waveform: 2-level grayscale screen update without flashing screen.
    /// - Gray update waveform: 8 or 16-level grayscale screen update without flashing screen
    /// - Gray clear update waveform: 8/16 gray level screen update with flashing screen
    ///
    enum Waveform
    {
        INVALID = 0,    ///< For initial value.
        DW,             ///< Direct waveform.
        GU,             ///< Gray update waveform.
        GC,             ///< Gray clear update waveform depends on system configuration.
        GC4,            ///< 4 Gray scale update waveform.
        GC8,            ///< 8 Gray scale update waveform.
        GC16,           ///< 16 Gray scale update wvaeform.
    };

    /// Define waveform selection policy.
    /// Waveform selection order
    /// - Check policy at first, if policy is specified, use the specified policy.
    /// - If policy is not specified, use default waveform
    enum WaveformPolicy
    {
        INVALID_POLICY = 0,
        PERFORMANCE_FIRST,
        QUALITY_FIRST,
    };

    static ScreenProxy & instance()
    {
        static ScreenProxy instance_;
        return instance_;
    }
    ~ScreenProxy();

    bool isUpdateEnabled();
    void enableUpdate(bool enable);

    void ensureUpdateFinished();

    void setDefaultWaveform(Waveform w = onyx::screen::ScreenProxy::GC);
    Waveform defaultWaveform() const;
    Waveform toggleWaveform();
    Waveform saveWaveform();
    Waveform restoreWaveform();

    void setWaveformPolicy(WaveformPolicy policy = QUALITY_FIRST);
    WaveformPolicy waveformPolicy();

    Waveform waveform();
    int & userData() { return user_data_; }

    void flush(const QWidget *widget,
               Waveform w = INVALID,
               bool whole = true,
               ScreenCommand::WaitMode wait = ScreenCommand::WAIT_BEFORE_UPDATE);
    void flush(int timeout = 3000);

    QRect & sync(const QWidget *widget, const QRect * region = 0);
    void updateWidget(const QWidget *widget,
                      Waveform w = INVALID,
                      bool whole = true,
                      ScreenCommand::WaitMode wait = ScreenCommand::WAIT_BEFORE_UPDATE);
    void updateWidgetRegion(const QWidget *,
                            const QRect&,
                            Waveform w = INVALID,
                            bool whole = true,
                            ScreenCommand::WaitMode wait = ScreenCommand::WAIT_BEFORE_UPDATE);
    void updateScreen(Waveform w = INVALID,
                      ScreenCommand::WaitMode wait = ScreenCommand::WAIT_BEFORE_UPDATE);

    void drawLine(int x1, int y1, int x2, int y2, unsigned char color, int size);
    void drawLines(QPoint * points, const int size, unsigned char color, int width);
    void fillScreen(unsigned char color);

    void setGCInterval(const int interval);
    void resetGUCount();
    void updateWidgetWithGCInterval(const QWidget *widget,
                                   const QRect * rect = 0,
                                   Waveform w = INVALID,
                                   bool whole = true,
                                   ScreenCommand::WaitMode wait = ScreenCommand::WAIT_BEFORE_UPDATE);

private:
    ScreenProxy();
    ScreenProxy(ScreenProxy & ref){}

    QRect & screenRegion(const QWidget *widget, const QRect * region = 0);

    bool enable_update_;    ///< Enable update or not.
    WaveformPolicy policy_; ///< Waveform selection policy
    Waveform waveform_;     ///< Default update waveform for normal use.
    Waveform previous_waveform_;     ///< Stored waveform.
    QRect rect_;            ///< The update region.
    int user_data_;         ///< User data.
    int gc_interval_;     ///< The interval for screen update with GC waveform
    int gu_count_;              ///< Count the screen update with GU waveform

private:
    void increaseGUCount();

};

/// Is screen update enabled.
inline bool ScreenProxy::isUpdateEnabled()
{
    return enable_update_;
}

/// Enable or disable the screen update.
inline void ScreenProxy::enableUpdate(bool enable)
{
    enable_update_ = enable;
}

/// Get the default update type.
inline ScreenProxy::Waveform ScreenProxy::defaultWaveform() const
{
    return waveform_;
}

/// Change the default update type according to current update type.
inline ScreenProxy::Waveform ScreenProxy::toggleWaveform()
{
    if (waveform_ == ScreenProxy::GC)
    {
        waveform_ = ScreenProxy::GU;
    }
    else
    {
        waveform_ = ScreenProxy::GC;
    }
    return waveform_;
}

/// Store current waveform.
inline ScreenProxy::Waveform ScreenProxy::saveWaveform()
{
    previous_waveform_ = waveform_;
    return waveform_;
}

/// Restore the waveform to previous stored type.
inline ScreenProxy::Waveform ScreenProxy::restoreWaveform()
{
    waveform_ = previous_waveform_;
    return waveform_;
}

/// Convenience function for ScreenManager::instance.
inline ScreenProxy & instance()
{
    return ScreenProxy::instance();
}

}  // namespace screen.
}  // namespace onyx

#endif
