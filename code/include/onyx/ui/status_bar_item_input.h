#ifndef ONYX_STATUS_BAR_ITEM_INPUT_H_
#define ONYX_STATUS_BAR_ITEM_INPUT_H_

#include "status_bar_item.h"

namespace ui
{

class StatusBarItemInput : public StatusBarItem
{
    Q_OBJECT

public:
    StatusBarItemInput(QWidget *parent);
    virtual ~StatusBarItemInput(void);

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

#endif  // ONYX_STATUS_BAR_ITEM_INPUT_URL_H_
