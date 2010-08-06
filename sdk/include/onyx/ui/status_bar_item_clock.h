#ifndef ONYX_STATUS_BAR_ITEM_CLOCK_H_
#define ONYX_STATUS_BAR_ITEM_CLOCK_H_

#include "status_bar_item.h"

namespace ui
{

class StatusBarItemClock : public StatusBarItem
{
    Q_OBJECT

public:
    StatusBarItemClock(QWidget *parent);
    virtual ~StatusBarItemClock(void);

public:
    QDateTime & startDateTime() { return start_; }

Q_SIGNALS:
    void clicked();

private:
    virtual void paintEvent(QPaintEvent *);
    virtual void mousePressEvent(QMouseEvent *);
    virtual void mouseReleaseEvent(QMouseEvent *);

    void createLayout();
    QImage & image();
    QString resourcePath();

private:
    Images images_;
    QDateTime start_;
};

};  // namespace ui

#endif  // ONYX_STATUS_BAR_ITEM_CLOCK_H_
