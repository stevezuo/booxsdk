#ifndef ONYX_TREE_VIEW_DIALOG_H_
#define ONYX_TREE_VIEW_DIALOG_H_

#include "onyx/base/base.h"
#include "ui_global.h"
#include "tree_view.h"
#include "buttons.h"
#include "label.h"
#include "status_bar.h"

namespace ui
{

/// The Tree View Dialog is designed to display both linear and hierarchy
/// data structure, such as bookmark list and table of content.
class TreeViewDialog : public QDialog
{
    Q_OBJECT
public:
    explicit TreeViewDialog(QWidget *parent, QStandardItemModel & model);
    explicit TreeViewDialog(QWidget *parent);
    ~TreeViewDialog(void);

public:
    void setModel(QStandardItemModel * model);
    int popup(const QString & title = "", const QModelIndex &idx = QModelIndex());
    void setTitleIcon(const QString & path);
    QModelIndex selectedItem();

    OnyxTreeView & tree() { return tree_; }

private:
    void keyPressEvent(QKeyEvent *ke);
    void keyReleaseEvent(QKeyEvent *ke);
    bool event(QEvent *event);
    void createLayout();
    void setColumnWidth();

private Q_SLOTS:
    void onStatusBarClicked(const int, const int);
    void onItemActivated(const QModelIndex &);
    void onCloseClicked(bool);

protected:
    OnyxPushButton  close_button_;
    ui::OnyxTreeView tree_;
    StatusBar        status_bar_;    ///< Status Bar
    QModelIndex      selected_;

    QVBoxLayout      layout_;        ///< Widgets Layout
    QHBoxLayout      title_layout_;
    OnyxLabel       title_icon_;
    OnyxLabel       title_label_;   ///< Title lable.
    QFrame           hor_separator_;

private:
    NO_COPY_AND_ASSIGN(TreeViewDialog);
};

} //namespace ui

#endif  // ONYX_TREE_VIEW_DIALOG_H_
