#include "onyx/ui/single_shot_timer.h"

namespace ui
{

OnyxSingleShotTimer::OnyxSingleShotTimer(int msec, QObject * receiver, const char * member)
    : QObject(0)
    , timer_(0)
    , msec_(msec)
    , receiver_(receiver)
    , member_(member)
{
}

OnyxSingleShotTimer::~OnyxSingleShotTimer()
{
}

void OnyxSingleShotTimer::start()
{
    timer_.reset(new QTimer());

    // connect signal
    timer_->setSingleShot(true);
    timer_->setInterval(msec_);
    connect(timer_.get(), SIGNAL(timeout()), receiver_, member_);

    timer_->start();
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