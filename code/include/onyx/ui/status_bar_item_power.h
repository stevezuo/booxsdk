#ifndef ONYX_STATUS_BAR_ITEM_BATTERY_H_
#define ONYX_STATUS_BAR_ITEM_BATTERY_H_

#include "status_bar_item.h"

namespace ui
{

class StatusBarItemBattery : public StatusBarItem
{
    Q_OBJECT

public:
    StatusBarItemBattery(QWidget *parent);
    virtual ~StatusBarItemBattery(void);

public Q_SLOTS:
    bool setBatteryStatus(const int value, const int status);

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
    int value_;
    int status_;
};

};  // namespace ui

#endif  // ONYX_STATUS_BAR_ITEM_BATTERY_H_
