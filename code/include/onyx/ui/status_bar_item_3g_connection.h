#ifndef ONYX_STATUS_BAR_ITEM_3G_CONNECTION_H_
#define ONYX_STATUS_BAR_ITEM_3G_CONNECTION_H_

#include "status_bar_item.h"

namespace ui
{

class StatusBarItem3GConnection : public StatusBarItem
{
    Q_OBJECT

public:
    StatusBarItem3GConnection(QWidget *parent);
    virtual ~StatusBarItem3GConnection(void);

public Q_SLOTS:
    void setConnectionStatus(const int status);
    void onSignalStrengthChanged(const int signal, const int total, const int network);

private:
    virtual void paintEvent(QPaintEvent *);
    QImage & image();
    QString resourcePath();

private:
    Images images_;
    int status_;
};

};      // namespace ui

#endif      // ONYX_STATUS_BAR_ITEM_3G_CONNECTION_H_
