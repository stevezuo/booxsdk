#ifndef ONYX_STATUS_BAR_ITEM_STYLUS_H_
#define ONYX_STATUS_BAR_ITEM_STYLUS_H_

#include "status_bar_item.h"

namespace ui
{

class StatusBarItemStylus : public StatusBarItem
{
    Q_OBJECT

public:
    StatusBarItemStylus(QWidget *parent);
    virtual ~StatusBarItemStylus(void);

public Q_SLOTS:
    void setStylusState(const int state);

private:
    virtual void paintEvent(QPaintEvent *);
    QImage & image();
    QString resourcePath();

private:
    Images images_;
};

};  // namespace ui

#endif  // ONYX_STATUS_BAR_ITEM_STYLUS_H_
