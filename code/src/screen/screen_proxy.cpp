// -*- mode: c++; c-basic-offset: 4; -*-

#include "onyx/screen/screen_proxy.h"

#include <QtGui/QtGui>
#ifdef ENABLE_EINK_SCREEN
#include <QtGui/qscreen_qws.h>
#endif

namespace onyx
{
namespace screen
{

const int PORT = 5678;

static ScreenCommand command_;
static QUdpSocket socket_;

inline static void connect()
{
    socket_.connectToHost(QHostAddress(QHostAddress::LocalHost), PORT);
}

// TODO: sendCommand should only take one argument. The second is redundant.
inline static void sendCommand(
        ScreenCommand & command,
        ScreenCommand::WaitMode wait = ScreenCommand::WAIT_BEFORE_UPDATE)
{
    // qDebug("sendCommand at %s", qPrintable(QTime::currentTime().toString("mm:ss.zzz")));
    command.wait_flags = wait;
    socket_.write(reinterpret_cast<const char *>(&command), sizeof(command));
    if (wait & ScreenCommand::WAIT_COMMAND_FINISH)
    {
        if (socket_.waitForReadyRead(3000))
        {
            socket_.read(reinterpret_cast<char *>(&command), sizeof(command));
        }
    }
}

ScreenProxy::ScreenProxy()
: enable_update_(true)
, policy_(INVALID_POLICY)
  , waveform_(ScreenProxy::GC)
  , previous_waveform_(ScreenProxy::GC)
  , user_data_(0)
  , gc_interval_(1)
  , gu_count_(0)
{
    connect();
}

ScreenProxy::~ScreenProxy()
{
}

void ScreenProxy::setGCInterval(const int interval)
{
    this->gc_interval_ = interval;
}

/// Make sure the previous update request has been processed.
void ScreenProxy::ensureUpdateFinished()
{
    command_.type = ScreenCommand::WAIT_FOR_FINISHED;
    sendCommand(command_);
}

/// Change the default screen update waveform to specified type.
/// \param w The new default waveform type.
void ScreenProxy::setDefaultWaveform(Waveform w)
{
    if (w == INVALID)
    {
        waveform_ = ScreenProxy::GC;
    }
    else
    {
        waveform_ = w;
    }
}

/// Calculate the region from widget coordinates to screen coordinates.
/// \param widget The Qt widget.
/// \param region The result in screen coordinates system.
QRect & ScreenProxy::screenRegion(const QWidget *widget,
                                  const QRect *region)
{
#ifndef BUILD_FOR_ARM
    rect_.setCoords(0, 0, 0, 0);
    return rect_;
#endif
    int degree = 0;

    // Get widget screen rectangle.
    QRect desk = qApp->desktop()->screenGeometry();
    rect_ = desk;

    if (widget)
    {
        rect_.moveTopLeft(widget->mapToGlobal(widget->rect().topLeft()));
        rect_.setSize(widget->size());
    }

    // combine the region
    if (region)
    {
        rect_.moveTo(region->topLeft());
        rect_.setSize(region->size());
    }

#ifdef BUILD_FOR_ARM
    degree = QScreen::instance()->transformOrientation() * 90;
#endif

    if (degree == 90)
    {
        rect_.setRect(rect_.y(),
                      desk.width() - rect_.x() - rect_.width(),
                      rect_.height(),
                      rect_.width());
        desk.setSize(QSize(desk.height(), desk.width()));
    }
    else if (degree == 180)
    {
        qDebug("destk %d %d rect %d %d %d %d", desk.width(), desk.height(), rect_.left(), rect_.top(), rect_.width(), rect_.height());

        rect_.setRect(desk.width() - rect_.x() - rect_.width(),
                      desk.height() - rect_.y() - rect_.height(),
                      rect_.width(),
                      rect_.height());
        desk.setSize(QSize(desk.width(), desk.height()));
    }
    else if (degree == 270)
    {
        rect_.setRect(desk.height() - rect_.y() - rect_.height(),
                      rect_.x(),
                      rect_.height(),
                      rect_.width());
        desk.setSize(QSize(desk.height(), desk.width()));
    }

    rect_ = rect_.intersected(desk);
    return rect_;
}

/// Ensure all pending events have been processed and update screen
/// for the given widget.
/// \widget The widget needs to update.
/// \w The waveform used to update screen.
/// \whole Update whole screen or not.
/// \wait The synchronous way between ScreenProxy and server.
void ScreenProxy::flush(const QWidget *widget,
                        Waveform w,
                        bool whole,
                        ScreenCommand::WaitMode wait)
{
    flush();
    updateWidget(widget, w, whole, wait);
}

/// Process all pending requests without updating screen.
void ScreenProxy::flush(int timeout)
{
    enableUpdate(false);
    QTime t;
    t.start();
    while (QApplication::hasPendingEvents() && t.elapsed() <= timeout)
    {
        QApplication::processEvents();
    }
    enableUpdate(true);
}

/// This function asks screen daemon to copy data from
/// Qt framebuffer to display controller.
/// \param widget The source widget to copy data from.
/// \param region The region inside the widget. If it's NULL, all data in
/// widget is copied to display controller.
QRect & ScreenProxy::sync(const QWidget *widget,
                          const QRect * region)
{
    QRect & rc = screenRegion(widget, region);
    command_.type = ScreenCommand::SYNC;
    command_.top = rc.top();
    command_.left = rc.left();
    command_.width = rc.width();
    command_.height = rc.height();
    sendCommand(command_);
    return rc;
}

/// This function copies data for the specified widget from Qt framebuffer
/// to display controller and updates the screen by using the waveform.
/// \param widget The Qt widget to udpate.
/// \param waveform The waveform to update screen.
/// \param update_whole To update whole screen or only the region occupied by the widget.
/// \param wait The synchronous way between ScreenProxy and server.
void ScreenProxy::updateWidget(const QWidget *widget,
                               Waveform waveform,
                               bool update_whole,
                               ScreenCommand::WaitMode wait)
{
    if (!isUpdateEnabled() && waveform != onyx::screen::ScreenProxy::DW)
    {
        return;
    }

    QRect & rc = screenRegion(widget, 0);
    if (waveform == INVALID)
    {
        waveform = waveform_;
    }

    // Construct command.
    command_.type = ScreenCommand::SYNC_AND_UPDATE;
    command_.top = rc.top();
    command_.left = rc.left();
    command_.width = rc.width();
    command_.height = rc.height();
    command_.waveform = waveform;
    if (update_whole)
    {
        command_.update_flags = ScreenCommand::FULL_UPDATE;
    }
    else
    {
        command_.update_flags = ScreenCommand::PARTIAL_UPDATE;
    }
    sendCommand(command_, wait);
}

/// Update the specified region of the widget.
/// \param widget The target Qt widget.
/// \param rect The region inside the widget.
/// \param waveform The waveform used to update screen. If it's INVALID, the default
/// waveform is applied.
/// \param update_whole Update whole screen or not.
/// \param wait The synchronous way between ScreenProxy and server.
void ScreenProxy::updateWidgetRegion(const QWidget *widget,
                                     const QRect &rect,
                                     Waveform waveform,
                                     bool update_whole,
                                     ScreenCommand::WaitMode wait)
{
    if (!isUpdateEnabled() && waveform != onyx::screen::ScreenProxy::DW)
    {
        return;
    }

    QRect & rc = screenRegion(widget, &rect);
    if (waveform == INVALID)
    {
        waveform = waveform_;
    }

    // Construct command.
    command_.type = ScreenCommand::SYNC_AND_UPDATE;
    command_.top = rc.top();
    command_.left = rc.left();
    command_.width = rc.width();
    command_.height = rc.height();
    command_.waveform = waveform;
    if (update_whole)
    {
        command_.update_flags = ScreenCommand::FULL_UPDATE;
    }
    else
    {
        command_.update_flags = ScreenCommand::PARTIAL_UPDATE;
    }
    sendCommand(command_, wait);
}

/// Update screen without copying any data.
/// \param waveform The waveform to udpate screen.
/// \param wait The synchronous mode between ScreenProxy and server.
void ScreenProxy::updateScreen(Waveform waveform,
                               ScreenCommand::WaitMode wait)
{
    // Construct command.
    command_.type = ScreenCommand::UPDATE;
    if (waveform == INVALID)
    {
        waveform = waveform_;
    }
    command_.waveform = waveform;
    command_.update_flags = ScreenCommand::FULL_UPDATE;
    sendCommand(command_, wait);
}

/// Draw line on screen directly.
/// \param x1 The x position of start point in screen coordinates.
/// \param y1 The y position of start point in screen coordinates.
/// \param x2 The x position of end point in screen coordinates.
/// \param y2 The y position of start point in screen coordinates.
/// \param color The line color value in grey level.
/// \param size The line width.
void ScreenProxy::drawLine(int x1, int y1, int x2, int y2,
                           unsigned char color,
                           int size)
{
    command_.type = ScreenCommand::DRAW_LINE;
    command_.points[0].rx() = x1;
    command_.points[0].ry() = y1;
    command_.points[1].rx() = x2;
    command_.points[1].ry() = y2;
    command_.color = color;
    command_.size = size;
    sendCommand(command_, ScreenCommand::WAIT_NONE);
}

/// Draw lines on screen directly.
/// \param points The points in screen coordinates.
/// \param color The lines color value in grey level.
/// \param size The lines width.
void ScreenProxy::drawLines(QPoint * points,
                            const int size,
                            unsigned char color,
                            int width)
{
    if (size > ScreenCommand::MAX_POINTS)
    {
        return;
    }

    command_.type = ScreenCommand::DRAW_LINES;
    memcpy(&command_.points, points, sizeof (QPoint) * size);
    command_.point_count = size;
    command_.color = color;
    command_.size = width;
    sendCommand(command_, ScreenCommand::WAIT_NONE);
}

/// Fill screen by using the specified color.
/// \param color The color value in grey level.
void ScreenProxy::fillScreen(unsigned char color)
{
    command_.type = ScreenCommand::FILL_SCREEN;
    command_.color = color;
    command_.waveform = ScreenProxy::GC;
    command_.update_flags = ScreenCommand::FULL_UPDATE;
    sendCommand(command_);
}

void ScreenProxy::setWaveformPolicy(WaveformPolicy policy)
{
    policy_ = policy;
}

ScreenProxy::WaveformPolicy ScreenProxy::waveformPolicy()
{
    return policy_;
}

void ScreenProxy::updateWidgetWithGCInterval(const QWidget *widget,
        const QRect *rect,
        Waveform waveform,
        bool update_whole,
        ScreenCommand::WaitMode wait)
{
    if (INVALID == waveform || GU == waveform)
    {
        waveform = GU;
        increaseGUCount();
        if (0 != gc_interval_ && gu_count_ >= gc_interval_)
        {
            waveform = GC;
        }
    }
    if (GC == waveform)
    {
        resetGUCount();
    }
    if (NULL == rect)
    {
        updateWidget(widget, waveform, update_whole, wait);
    }
    else
    {
        updateWidgetRegion(widget, *rect, waveform, update_whole, wait);
    }
}

void ScreenProxy::increaseGUCount()
{
    ++gu_count_;
}

void ScreenProxy::resetGUCount()
{
    gu_count_ = 0;
}

}  //namespace screen

}  //namespace onyx
