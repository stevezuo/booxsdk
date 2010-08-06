#ifndef ONYX_TREE_VIEW_H_
#define ONYX_TREE_VIEW_H_

#include "onyx/base/base.h"
#include <QtGui/QtGui>

namespace ui
{

/// Onyx tree view for eink device.
class OnyxTreeHeaderBar : public QWidget
{
    Q_OBJECT
public:
    OnyxTreeHeaderBar(QWidget *parent, QStandardItemModel * model);
    ~OnyxTreeHeaderBar();

public:
    void setModel(QStandardItemModel * model);
    void setColumnWidth(const QVector<int> &percentages);
    const QVector<int> & columnWidth() { return percentages_; }
    QRect columnRect(int column);
    QStandardItemModel * model() { return model_; }

public Q_SLOTS:
    void clear();

protected:
    virtual void paintEvent(QPaintEvent *);

private:
    QStandardItemModel * model_;
    QVector<int> percentages_;
};

class OnyxTreeView;
class OnyxTreeViewItem : public QWidget
{
    Q_OBJECT
public:
    OnyxTreeViewItem(QWidget *parent, OnyxTreeView & view);
    ~OnyxTreeViewItem();

public:
    void setData(QStandardItem *item);
    QStandardItem * data() { return data_; }

    bool isSelected() { return selected_; }
    void select(bool select);

    bool needRepaint() const { return need_repaint_;}

Q_SIGNALS:
    void pressed(OnyxTreeViewItem *item, const QPoint & press);
    void released(OnyxTreeViewItem *item, const QPoint & release);
    void clicked(OnyxTreeViewItem *item);

private:
    virtual void mousePressEvent(QMouseEvent *);
    virtual void mouseReleaseEvent(QMouseEvent *);
    virtual void keyPressEvent(QKeyEvent *);
    virtual void keyReleaseEvent(QKeyEvent *);
    virtual void paintEvent(QPaintEvent *);
    virtual void resizeEvent(QResizeEvent *);

private:
    QStandardItem * data_;
    bool selected_;
    bool need_repaint_;
    OnyxTreeView & view_;
};

/// Tree view for eink device. Remove unnecessary updates.
/// The tree view also implements necessary pagination.
/// The basic idea is to reuse existing QTreeWidget.
/// It creates internal model with only visible paginated items.
/// When location changed, it updates these visible items and
/// updates the internal tree widget.
class OnyxTreeView : public QWidget
{
    Q_OBJECT
public:
    OnyxTreeView(QWidget *parent, QStandardItemModel * model);
    ~OnyxTreeView();

public:
    int  pages();
    bool jumpToPage(int page);
    int  currentPage();
    int  itemsPerPage();

    bool pageUp();
    bool pageDown();

    void showHeader(bool show);
    OnyxTreeHeaderBar & header() { return header_bar_; }

    void setModel(QStandardItemModel * model);
    QStandardItem * item(QStandardItem *item, int col);

public Q_SLOTS:
    void clear();
    bool select(const QModelIndex & index);
    bool select(const QString & title);
    bool setColumnWidth(const QVector<int> &percentages);
    void keyReleaseEvent(QKeyEvent * event);

protected:
    virtual void keyPressEvent(QKeyEvent * event);
    virtual void resizeEvent(QResizeEvent *e);
    virtual void paintEvent(QPaintEvent *);

Q_SIGNALS:
    void activated(const QModelIndex & index);
    void positionChanged(int current, int total);
    void exceed(bool begin);

private Q_SLOTS:
    void onItemClicked(OnyxTreeViewItem *);
    void onItemPressed(OnyxTreeViewItem *, const QPoint &);
    void onItemReleased(OnyxTreeViewItem *, const QPoint &);

private:
    void createLayout();
    void setupInternalModel(QStandardItemModel * model,
                            QStandardItem *item,
                            int level);
    void arrangeItems(int, int, int);
    void updateLayout(const int rows);
    bool selectItem(OnyxTreeViewItem *item);
    QStandardItem * item(int row, int col);
    bool navigate(int offset);

    QString level(int row, int col);

    void updateTreeWidget();
    int first_visible();
    void reportPosition();

    void activate(int select = -1);

private:
    QVBoxLayout layout_;
    QStandardItemModel *model_;
    QGridLayout items_layout_;

    OnyxTreeHeaderBar header_bar_;

    int selected_;      ///< Absolute position.
    int first_visible_; ///< Absolute position.
    int items_per_page_;
    QVector<QStandardItem *> all_items_;

    typedef shared_ptr<OnyxTreeViewItem> ViewItemPtr;
    typedef vector<ViewItemPtr> ViewItemPtrs;
    typedef vector<ViewItemPtr>::iterator ViewItemPtrIter;
    ViewItemPtrs views_;

    OnyxTreeViewItem *pressed_item_;
    QPoint pressed_point_;
};

}   // namespace ui

#endif  // ONYX_TREE_VIEW_H_
