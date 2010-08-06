#ifndef ONYX_STATUS_BAR_ITEM_TEXT_H_
#define ONYX_STATUS_BAR_ITEM_TEXT_H_

#include "status_bar_item.h"

namespace ui
{

class StatusBarItemMessage : public StatusBarItem
{
    Q_OBJECT

public:
    StatusBarItemMessage(QWidget *parent);
    virtual ~StatusBarItemMessage(void);

    QSize sizeHint() const;
    QSize minimumSizeHint() const;

public Q_SLOTS:
    void setMessage(int current, int total);
    void setMessage(const QString & message);

Q_SIGNALS:
    void clicked();

private:
    virtual void mousePressEvent(QMouseEvent *);
    virtual void mouseMoveEvent(QMouseEvent *);
    virtual void mouseReleaseEvent(QMouseEvent *);
    virtual void paintEvent(QPaintEvent *);
    void paintText(QPainter &painter);

private:
    QString message_;
    scoped_ptr<QFontMetrics> metrics_;
};

};  // namespace ui

#endif  // ONYX_STATUS_BAR_ITEM_TEXT_H_
