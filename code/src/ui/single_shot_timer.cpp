#include "onyx/ui/single_shot_timer.h"

namespace ui
{

OnyxSingleShotTimer::OnyxSingleShotTimer(int msec, QObject * receiver, const char * member)
    : QObject(0)
    , timer_(new QTimer())
    , msec_(msec)
    , receiver_(receiver)
    , member_(member)
{
    // connect signal
    timer_->setSingleShot(true);
    timer_->setInterval(msec_);
    connect(timer_.get(), SIGNAL(timeout()), receiver_, member_);
}

OnyxSingleShotTimer::~OnyxSingleShotTimer()
{
}

void OnyxSingleShotTimer::start()
{
    if (timer_ != 0)
    {
        timer_->start();
    }
}

void OnyxSingleShotTimer::stop()
{
    if (timer_ != 0)
    {
        timer_->stop();
    }
}

void OnyxSingleShotTimer::setInterval(int msec)
{
    msec_ = msec;
    if (timer_ != 0)
    {
        timer_->setInterval(msec_);
    }
}

}