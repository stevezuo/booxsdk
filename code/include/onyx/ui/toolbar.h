#ifndef ONYX_TOOL_BAR_H_
#define ONYX_TOOL_BAR_H_

#include "onyx/base/base.h"
#include <QtGui/QtGui>

namespace ui
{

class OnyxToolBarItem : public QWidget
{
    Q_OBJECT
public:
    OnyxToolBarItem(QWidget *parent, QAction &action);
    ~OnyxToolBarItem();

Q_SIGNALS:
    void clicked(QAction *action);

private Q_SLOTS:
    void onActionChanged();

private:
    QSize iconSize();
    void mousePressEvent(QMouseEvent *me);
    void mouseReleaseEvent(QMouseEvent *me);
    void paintEvent(QPaintEvent *pe);

private:
    QAction &action_;
    int state_;
};
typedef shared_ptr<OnyxToolBarItem> OnyxToolBarItemPtr;

/// Tool bar for eink device. Remove unnecessary updates.
class OnyxToolBar : public QWidget
{
public:
    explicit OnyxToolBar(QWidget *parent = 0);
    ~OnyxToolBar();

public:
    void addAction(QAction *action);

protected:
    bool event(QEvent *e);
    void paintEvent(QPaintEvent *pe);

private:
    std::vector<OnyxToolBarItemPtr> items_;
    QHBoxLayout layout_;

};

}; // namespace ui

#endif  // ONYX_TOOL_BAR_H_
