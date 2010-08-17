#ifndef ONYX_UI_EVENT_FILTER_H_
#define ONYX_UI_EVENT_FILTER_H_

#include <QtGui/QtGui>

namespace ui
{

class EventFilter : public QObject
{
    Q_OBJECT

public:
    EventFilter();
    ~EventFilter();

public Q_SLOTS:
    void defaultKeys(QList<int> & press_keys, QList<int> & release_keys);
    void addObject(QObject *object, const QList<int> & press_keys, const QList<int> & release_keys);
    void removeObject(QObject *object);

Q_SIGNALS:
    void pressed(QObject *object, int key);
    void released(QObject *object, int key);

private:
    bool eventFilter(QObject *obj, QEvent *event);

private:
    struct Keys
    {
        QList<int> press;
        QList<int> release;
    };

    QMap<QObject *, Keys> objects_;
};

};

#endif  // ONYX_UI_EVENT_FILTER_H_

