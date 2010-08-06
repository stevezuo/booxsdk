
#include <QtGui/QtGui>
#include "onyx/ui/tree_view.h"
#include "onyx/ui/text_layout.h"
#include "onyx/sys/sys.h"
#include "onyx/screen/screen_proxy.h"

namespace ui
{

static const int LEVEL_INDEX = Qt::UserRole + 20;
static const int ITEM_HEIGHT = 36;
static const int HORIZONTAL_SPACING = 11;
static const int MARGIN = 2;

static QFont tree_font;

static void setLevel(QStandardItem *item, int level)
{
    if (item)
    {
        item->setData(level, LEVEL_INDEX);
    }
}

static int level(QStandardItem *item)
{
    if (item)
    {
        return item->data(LEVEL_INDEX).toInt();
    }
    return 0;
}

static int indent(QStandardItem *item)
{
    return ui::level(item) * 30;
}

OnyxTreeHeaderBar::OnyxTreeHeaderBar(QWidget *parent, QStandardItemModel * model)
: QWidget(parent)
, model_(model)
{
    setFixedHeight(50);

    setModel(model);
}

OnyxTreeHeaderBar::~OnyxTreeHeaderBar()
{
}

void OnyxTreeHeaderBar::clear()
{
    percentages_.clear();
    model_ = 0;
}

void OnyxTreeHeaderBar::setModel(QStandardItemModel * model)
{
    percentages_.clear();
    if (model)
    {
        for(int i = 0; i < model->columnCount(); ++i)
        {
            percentages_.push_back(100 / model->columnCount());
        }
    }
    model_ = model;
}

void OnyxTreeHeaderBar::setColumnWidth(const QVector<int> &percentages)
{
    percentages_ = percentages;
}

QRect OnyxTreeHeaderBar::columnRect(int column)
{
    int left = 0, right = 0;
    parentWidget()->layout()->getContentsMargins(&left, 0, &right, 0);
    QRect rc(parentWidget()->rect());
    int width = parentWidget()->width() - left - right;
    int available_width = width - (percentages_.size() - 1) * HORIZONTAL_SPACING;
    int x = 0;
    for(int i = 0; i < column; ++i)
    {
        x += percentages_[i] * available_width / 100;
        x += HORIZONTAL_SPACING;
    }
    int w = percentages_[column] * available_width / 100;
    rc.setLeft(x);
    rc.setWidth(w);
    rc.setHeight(height() - 6);
    return rc;
}

void OnyxTreeHeaderBar::paintEvent(QPaintEvent *)
{
    if (model_ == 0)
    {
        return;
    }

    QPainter p(this);

    for(int i = 0; i < model_->columnCount(); ++i)
    {
        QRect rc(columnRect(i));
        QPainterPath path;
        path.addRoundedRect(rc, 4, 4, Qt::AbsoluteSize);
        p.fillPath(path, QBrush(QColor(193, 193, 193)));
        QString title = model_->headerData(i, Qt::Horizontal).toString();
        ui::drawSingleLineText(p, tree_font, title,
                               Qt::AlignCenter,
                               rc.adjusted(MARGIN, MARGIN, MARGIN,MARGIN));
    }

    // Draw the lines.
    int x1 = 0;
    int y1 = height() - 4;
    int x2 = width();
    p.setPen(QColor(124, 124, 124));
    p.drawLine(x1, y1, x2, y1);

    p.setPen(QColor(147, 147, 147));
    p.drawLine(x1, y1 + 1, x2, y1 + 1);

    p.setPen(QColor(185, 185, 185));
    p.drawLine(x1, y1 + 2, x2, y1 + 2);

}

OnyxTreeViewItem::OnyxTreeViewItem(QWidget *parent, OnyxTreeView & view)
: QWidget(parent)
, data_(0)
, selected_(false)
, need_repaint_(true)
, view_(view)
{
}

OnyxTreeViewItem::~OnyxTreeViewItem()
{
}

void OnyxTreeViewItem::setData(QStandardItem *item)
{
    data_ = item;
    need_repaint_ = true;
}

void OnyxTreeViewItem::select(bool select)
{
    if (selected_ == select)
    {
        return;
    }
    selected_ = select;
    need_repaint_ = true;
}

void OnyxTreeViewItem::mousePressEvent(QMouseEvent *me)
{
    emit pressed(this, me->globalPos());
}

void OnyxTreeViewItem::mouseReleaseEvent(QMouseEvent *me)
{
    emit released(this, me->globalPos());
}

void OnyxTreeViewItem::keyPressEvent(QKeyEvent *ke)
{
    ke->ignore();
}

void OnyxTreeViewItem::keyReleaseEvent(QKeyEvent *ke)
{
    if (ke->key() == Qt::Key_Return)
    {
        emit clicked(this);
    }
}

void OnyxTreeViewItem::paintEvent(QPaintEvent *)
{
    if (view_.header().model() == 0 || data() == 0)
    {
        return;
    }

    QPainter p(this);
    for(int i = 0; i < view_.header().model()->columnCount(); ++i)
    {
        QRect rc(view_.header().columnRect(i));
        rc.setTop(MARGIN);
        rc.setHeight(height() - 2 * MARGIN);
        QStandardItem * item = view_.item(data(), i);
        if (item != 0)
        {
            QString title = item->data(Qt::DisplayRole).toString();
            rc.adjust(MARGIN, MARGIN, -MARGIN, -MARGIN);

            if (i == 0)
            {
                rc.setLeft(rc.left() + ui::indent(item));
            }

            ui::drawSingleLineText(p, tree_font, title,
                                   item->textAlignment()|Qt::AlignVCenter,
                                   rc);
        }
    }

    if (isSelected())
    {
        QPen pen(Qt::SolidLine);
        p.setPen(Qt::black);
        p.drawRoundedRect(rect().adjusted(0, 0, -1, -1), 4, 4);
    }
}

void OnyxTreeViewItem::resizeEvent(QResizeEvent *)
{
}




OnyxTreeView::OnyxTreeView(QWidget *parent, QStandardItemModel * model)
: QWidget(parent)
, layout_(this)
, model_(model)
, items_layout_(0)
, header_bar_(0, model)
, selected_(-1)
, first_visible_(0)
, items_per_page_(1)
, all_items_()
, views_()
, pressed_item_(0)
, pressed_point_()
{
    setAutoFillBackground(true);
    setBackgroundRole(QPalette::Base);

    createLayout();
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    tree_font.setPointSize(20);

    if (model_)
    {
        setupInternalModel(model_, model_->invisibleRootItem(), -1);
    }
}

OnyxTreeView::~OnyxTreeView()
{
    clear();
}

void OnyxTreeView::createLayout()
{
    layout_.setContentsMargins(5, MARGIN, 5, MARGIN);
    layout_.setSpacing(0);
    layout_.addWidget(&header_bar_);
    layout_.addLayout(&items_layout_);
}

void OnyxTreeView::clear()
{
    header_bar_.clear();
    model_ = 0;
    selected_ = -1;
    first_visible_ = 0;
    items_per_page_ = 1;
    all_items_.clear();
}

void OnyxTreeView::onItemClicked(OnyxTreeViewItem *item)
{
    if (item->data() == 0)
    {
        return;
    }

    // Disable screen update now.
    onyx::screen::instance().enableUpdate(false);

    // Make the node selected.
    if (selectItem(item))
    {
        updateTreeWidget();
        onyx::screen::instance().flush(
            this,
            onyx::screen::ScreenProxy::DW,
            false,
            onyx::screen::ScreenCommand::WAIT_ALL);
    }
    onyx::screen::instance().enableUpdate(true);

    activate();
}

void OnyxTreeView::onItemPressed(OnyxTreeViewItem *item, const QPoint &point)
{
    pressed_item_ = item;
    pressed_point_ = point;
}

void OnyxTreeView::onItemReleased(OnyxTreeViewItem *item, const QPoint &point)
{
    int direction = sys::SystemConfig::direction(pressed_point_, point);

    if (direction == 0)
    {
        onItemClicked(item);
        return;
    }

    if (direction > 0)
    {
        pageDown();
    }
    else if (direction < 0)
    {
        pageUp();
    }
}

void OnyxTreeView::setModel(QStandardItemModel * model)
{
    header_bar_.setModel(model);
    model_ = model;
    all_items_.clear();
    setupInternalModel(model_, model_->invisibleRootItem(), -1);
    if (isVisible())
    {
        updateTreeWidget();
        reportPosition();
    }
}

QStandardItem * OnyxTreeView::item(QStandardItem *i, int col)
{
    if (col == 0)
    {
        return i;
    }

    QStandardItem * parent = i->parent();
    QStandardItem * child = 0;
    if (parent == 0)
    {
        child = model_->invisibleRootItem()->child(i->index().row(), col);
    }
    else
    {
        child = parent->child(i->index().row(), col);
    }
    return child;
}

/// This function returns the item specified by the row and col
/// in absolute position. The difference between this function and
/// normal QStandardItemModel is that the QStandardItemModel uses
/// relative row and col.
QStandardItem * OnyxTreeView::item(int row, int col)
{
    QStandardItem * item = all_items_[row];
    if (col == 0)
    {
        return item;
    }

    QStandardItem * parent = item->parent();
    QStandardItem * child = 0;
    if (parent == 0)
    {
        child = model_->invisibleRootItem()->child(item->index().row(), col);
    }
    else
    {
        child = parent->child(item->index().row(), col);
    }
    return child;
}

/// Return the level string for the specified row.
QString OnyxTreeView::level(int row, int col)
{
    QString string;
    if (col > 0)
    {
        return string;
    }

    int count = ui::level(all_items_[row]);
    for(int i = 0; i < count; ++i)
    {
        string += QString::fromLatin1("    ");
    }
    return string;
}

int OnyxTreeView::pages()
{
    int count = all_items_.size() / itemsPerPage();
    if (count * items_per_page_ == all_items_.size() && count != 0)
    {
        return count;
    }
    return count + 1;
}

bool OnyxTreeView::jumpToPage(int page)
{
    if (page < 1 || page > pages() || page == currentPage())
    {
        return false;
    }

    first_visible_  = (page - 1)* items_per_page_;
    selected_ = first_visible_;
    updateTreeWidget();
    reportPosition();
    return true;
}

int OnyxTreeView::currentPage()
{
    if (itemsPerPage() > 0)
    {
        return first_visible_ / items_per_page_ + 1;
    }
    return 1;
}

/// Search for the item specified by index and ensure the item is visible.
bool OnyxTreeView::select(const QModelIndex & index)
{
    selected_ = -1;
    for(int i = 0; i < all_items_.size(); ++i)
    {
        if (all_items_.at(i)->index() == index)
        {
            selected_ = i;
            break;
        }
    }

    if (selected_ < 0)
    {
        return false;
    }

    // Update tree widget if necessary.
    int new_page = selected_ / itemsPerPage() + 1;
    first_visible_ = (new_page - 1) * items_per_page_;
    if (isVisible())
    {
        updateTreeWidget();
        reportPosition();
    }
    return true;
}

/// Select item by title. Could be removed now.
bool OnyxTreeView::select(const QString & title)
{
    for(int i = 0; i < all_items_.size(); ++i)
    {
        if (all_items_.at(i)->text() == title)
        {
            return select(all_items_.at(i)->index());
        }
    }
    return false;
}

bool OnyxTreeView::setColumnWidth(const QVector<int> &percentages)
{
    // Ensure the columns are the same.
    if (model_->invisibleRootItem()->columnCount() != percentages.size())
    {
        return false;
    }
    header_bar_.setColumnWidth(percentages);
    return true;
}


void OnyxTreeView::keyPressEvent(QKeyEvent * event)
{
}

void OnyxTreeView::paintEvent(QPaintEvent *pe)
{
    QPainter p(this);

    if (hasFocus())
    {
        p.drawRect(rect());
    }
}

void OnyxTreeView::keyReleaseEvent(QKeyEvent * event)
{
    int key = event->key();
    switch (key)
    {
    case Qt::Key_PageDown:
        {
            event->accept();
            pageDown();
            return;
        }
    case Qt::Key_PageUp:
        {
            event->accept();
            pageUp();
            return;
        }
    case Qt::Key_Up:
        {
            event->accept();
            navigate(-1);
            return;
        }
    case Qt::Key_Down:
        {
            event->accept();
            navigate(1);
            return;
        }
    case Qt::Key_Enter:
    case Qt::Key_Return:
        {
            event->accept();
            activate();
            return;
        }
    default:
        break;
    }
    event->ignore();
}

bool OnyxTreeView::navigate(int offset)
{
    // Check new position is valid or not.
    int new_select = selected_ + offset;
    if (new_select < 0 ||
        new_select >= all_items_.size())
    {
        return false;
    }

    bool page_changed = false;
    selected_ = new_select;
    if (new_select < first_visible_)
    {
        first_visible_ -= itemsPerPage();
        page_changed = true;
    }
    else if (new_select >= first_visible_ + itemsPerPage())
    {
        first_visible_ += itemsPerPage();
        page_changed = true;
    }

    updateTreeWidget();

    if (page_changed)
    {
        reportPosition();
    }
    else
    {
        onyx::screen::instance().flush();
        onyx::screen::instance().updateWidget(
            this,
            onyx::screen::ScreenProxy::DW,
            false,
            onyx::screen::ScreenCommand::WAIT_ALL);
    }
    return true;
}

void OnyxTreeView::resizeEvent(QResizeEvent *e)
{
    // Need to calculate the first_visible according to the select item.
    if (selected_ < 0)
    {
        selected_ = 0;
    }

    first_visible_ = selected_ / itemsPerPage() * itemsPerPage();
    updateTreeWidget();
    updateLayout(itemsPerPage());
    reportPosition();
}

/// Recursively add the children item of index to internal model.
void OnyxTreeView::setupInternalModel(QStandardItemModel * model,
                                      QStandardItem *item,
                                      int level)
{
    if (item == 0 || model == 0)
    {
        return;
    }

    int count = item->rowCount();
    for(int i = 0; i < count; ++i)
    {
        QStandardItem * child = item->child(i, 0);
        setLevel(child, level + 1);
        all_items_.push_back(child);
        setupInternalModel(model, child, level + 1);
    }
}

void OnyxTreeView::arrangeItems(int first_position,
                                int selected,
                                int count)
{
    for(int i = static_cast<int>(views_.size()); i < count; ++i)
    {
        ViewItemPtr view(new OnyxTreeViewItem(this, *this));
        connect(view.get(),
                SIGNAL(clicked(OnyxTreeViewItem *)),
                this,
                SLOT(onItemClicked(OnyxTreeViewItem *)));

        connect(view.get(),
                SIGNAL(pressed(OnyxTreeViewItem *, const QPoint&)),
                this,
                SLOT(onItemPressed(OnyxTreeViewItem *, const QPoint&)));

        connect(view.get(),
                SIGNAL(released(OnyxTreeViewItem *, const QPoint&)),
                this,
                SLOT(onItemReleased(OnyxTreeViewItem *, const QPoint&)));

        views_.push_back(view);
        items_layout_.addWidget(view.get(), i, 0);
    }

    // Associate the node view with the node.
    int up = std::min(all_items_.size(), first_position + count);
    ViewItemPtrIter iter = views_.begin();
    for(int pos = first_position; pos < up; ++pos)
    {
        (*iter)->setData(all_items_[pos]);
        if (pos == selected)
        {
            (*iter)->select(true);
        }
        else
        {
            (*iter)->select(false);
        }

        if ((*iter)->needRepaint())
        {
            (*iter)->update();
        }
        ++iter;
    }

    // Update the other node views.
    ViewItemPtrIter end = views_.end();
    while (iter != end)
    {
        (*iter)->setData(0);
        if ((*iter)->needRepaint())
        {
            (*iter)->update();
        }
        ++iter;
    }
}

// Update visible items.
void OnyxTreeView::updateLayout(const int rows)
{
    assert(static_cast<int>(views_.size()) >= rows);

    if (items_layout_.count() > 0)
    {
        for(ViewItemPtrIter it = views_.begin();
            it != views_.end();
            ++it)
        {
            items_layout_.removeWidget(it->get());
            (*it)->hide();
        }
    }

    for(int i = 0; i < rows; ++i)
    {
        items_layout_.addWidget(views_[i].get(), i, 0);
        views_[i]->show();
    }
}

bool OnyxTreeView::selectItem(OnyxTreeViewItem *item)
{
    ViewItemPtrIter iter;
    for(iter = views_.begin(); iter != views_.end(); ++iter)
    {
        if (iter->get() == item)
        {
            break;
        }
    }

    if (iter == views_.end())
    {
        qDebug("item not found.");
        return false;
    }

    // Check if it's already selected.
    selected_ = iter - views_.begin() + first_visible_;
    return true;
}


/// Update the tree widget to use new model.
void OnyxTreeView::updateTreeWidget()
{
    arrangeItems(first_visible_, selected_, itemsPerPage());
}

int OnyxTreeView::first_visible()
{
    return first_visible_;
}

void OnyxTreeView::reportPosition()
{
    emit positionChanged(currentPage(), pages());
}

int OnyxTreeView::itemsPerPage()
{
    items_per_page_ = height() / ITEM_HEIGHT;
    if (items_per_page_ <= 0)
    {
        items_per_page_ = 1;
    }
    return items_per_page_;
}

bool OnyxTreeView::pageUp()
{
    if (first_visible_ <= 0)
    {
        emit exceed(true);
        return false;
    }

    first_visible_ -= items_per_page_;
    if (first_visible_ < 0)
    {
        first_visible_ = 0;
    }
    selected_ = first_visible_;
    updateTreeWidget();
    reportPosition();
    return true;
}

bool OnyxTreeView::pageDown()
{
    int new_pos = first_visible_ + items_per_page_;
    if (new_pos >= all_items_.size())
    {
        emit exceed(false);
        return false;
    }

    first_visible_ = new_pos;
    selected_ = first_visible_;
    updateTreeWidget();
    reportPosition();
    return true;
}

void OnyxTreeView::showHeader(bool visible)
{
    header_bar_.setVisible(visible);
}

void OnyxTreeView::activate(int select)
{
    if (select < 0)
    {
        if (selected_ >= 0 && selected_ < all_items_.size())
        {
            emit activated(all_items_.at(selected_)->index());
        }
    }
    else
    {
        emit activated(all_items_.at(select)->index());
    }
}

}

