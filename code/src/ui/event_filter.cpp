#include "onyx/ui/event_filter.h"

namespace ui
{

EventFilter::EventFilter()
{
}

EventFilter::~EventFilter()
{
}

void EventFilter::defaultKeys(QList<int> & press_keys,
                              QList<int> & release_keys)
{
    press_keys.clear();
    press_keys.push_back(Qt::Key_Up);
    press_keys.push_back(Qt::Key_Down);
    press_keys.push_back(Qt::Key_Left);
    press_keys.push_back(Qt::Key_Right);
    press_keys.push_back(Qt::Key_PageUp);
    press_keys.push_back(Qt::Key_PageDown);

    release_keys = press_keys;
}

void EventFilter::addObject(QObject *object,
                            const QList<int> & press_keys,
                            const QList<int> & release_keys)
{
    Keys d;
    d.press = press_keys;
    d.release = release_keys;
    objects_.insert(object, d);
    object->installEventFilter(this);
}

void EventFilter::removeObject(QObject *object)
{
    objects_.remove(object);
}

bool EventFilter::eventFilter(QObject *obj, QEvent *event)
{
    bool ret = QObject::eventFilter(obj, event);
    if (!objects_.contains(obj))
    {
        return ret;
    }

    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        Keys keys = objects_.value(obj);
        if (keys.press.contains(keyEvent->key()))
        {
            event->accept();
            emit pressed(obj, keyEvent->key());
            return true;
        }
    }
    else if (event->type() == QEvent::KeyRelease)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        Keys keys = objects_.value(obj);
        if (keys.release.contains(keyEvent->key()))
        {
            event->accept();
            emit released(obj, keyEvent->key());
            return true;
        }
    }
    return ret;

}


};


