#ifndef ONYX_STATUS_BAR_ITEM_VOLUME_H_
#define ONYX_STATUS_BAR_ITEM_VOLUME_H_

#include "status_bar_item.h"

namespace ui
{

class StatusBarItemVolume : public StatusBarItem
{
    Q_OBJECT

public:
    StatusBarItemVolume(QWidget *parent);
    virtual ~StatusBarItemVolume(void);

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
};

};  // namespace ui

#endif  // ONYX_STATUS_BAR_ITEM_VOLUME_H_
