#ifndef MENU_ITEM_H_
#define MENU_ITEM_H_

#include <QtCore/QtCore>
#include <QtGui/QtGui>
#include "context_dialog_base.h"

namespace ui
{

class MenuItem : public QWidget
{
    Q_OBJECT

public:
    MenuItem(QWidget *parent, QAction *action = 0);
    ~MenuItem(void);

public:
    QSize sizeHint() const;
    QSize minimumSizeHint() const;

    void setAction(QAction *action);
    QAction * action() { return action_; }

public Q_SLOTS:
    void click();
    void onActionChanged(bool);

Q_SIGNALS:
    void clicked(MenuItem* wnd, QAction *action);

protected:
    virtual bool event(QEvent *e);
    virtual void paintEvent(QPaintEvent *e);
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);

private:
    bool updateLayout();

    QSize iconActualSize() const;

private:
    QAction *action_;
    mutable QPoint icon_pos_;
    mutable QTextLayout title_layout_;
    mutable int layout_width_;
    mutable int layout_height_;
    bool is_layout_dirty_;

};
typedef std::vector<MenuItem *> MenuItems;
typedef MenuItems::iterator MenuItemsIter;

/// Manage MenuItem widgets.
class Section : public QObject
{
    Q_OBJECT
public:
    Section();
    ~Section();

public:
    int navigate(int);
    bool activate();
    void clear();

    bool setFocus(int row = 0, int col = 0);
    bool hasFocus();

    MenuItem *addItem(QWidget *parent, QAction *action, int row, int col);
    bool arrangeItems(QWidget *parent, BaseActions *items, int rows, int cols);

    QGridLayout & layout() { return layout_; }
    MenuItems & items() { return items_; }

Q_SIGNALS:
    void clicked(MenuItem *wnd, QAction *action);

private Q_SLOTS:
    void onClicked(MenuItem *wnd, QAction *action);

private:
    MenuItem *createItem(QWidget *parent, QAction *action);
    int focusItem();

    int prevRow(int pos);
    int prevColumn(int pos);
    int nextRow(int pos);
    int nextColumn(int pos);

private:
    QGridLayout layout_;
    int selected_row_;
    int selected_column_;
    MenuItems items_;
};

}

#endif
