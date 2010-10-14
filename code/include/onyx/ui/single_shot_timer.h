#ifndef ONYX_SINGLE_SHOT_TIMER_H_
#define ONYX_SINGLE_SHOT_TIMER_H_

#include <QtGui/QtGui>
#include "onyx/base/base.h"

namespace ui
{

class OnyxSingleShotTimer : public QObject
{
    Q_OBJECT
public:
    OnyxSingleShotTimer(int msec, QObject * receiver, const char * member);
    ~OnyxSingleShotTimer();

public:
    void start();
    void stop();
    void setInterval(int msec);

private:
    scoped_ptr<QTimer> timer_;
    int                msec_;
    QObject            *receiver_;
    const char         *member_;
};

};

#endif  // ONYX_SINGLE_SHOT_TIMER_H_
