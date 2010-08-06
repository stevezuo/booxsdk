#ifndef ONYX_STATUS_BAR_ITEM_CONNECTION_H_
#define ONYX_STATUS_BAR_ITEM_CONNECTION_H_

#include "status_bar_item.h"

namespace ui
{

class StatusBarItemConnection : public StatusBarItem
{
    Q_OBJECT

public:
    StatusBarItemConnection(QWidget *parent);
    virtual ~StatusBarItemConnection(void);

public Q_SLOTS:
    void setConnectionStatus(const int status);

private:
    virtual void paintEvent(QPaintEvent *);
    QImage & image();
    QString resourcePath();

private:
    Images images_;
    int status_;
};

};      // namespace ui

#endif      // ONYX_STATUS_BAR_ITEM_CONNECTION_H_
