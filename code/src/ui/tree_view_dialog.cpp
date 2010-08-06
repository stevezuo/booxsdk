#include "onyx/ui/tree_view_dialog.h"
#include "onyx/screen/screen_proxy.h"

namespace ui
{

TreeViewDialog::TreeViewDialog(QWidget *parent, QStandardItemModel & model)
#ifndef Q_WS_QWS
: QDialog(0)
#else
: QDialog(0, Qt::FramelessWindowHint)
#endif
, close_button_(QIcon(":/images/close.png"), "", 0)
, tree_(0, &model)
, status_bar_(0, ui::PROGRESS|ui::MESSAGE)
, layout_(this)
, title_layout_(0)
, title_icon_(0)
, title_label_(0)
, hor_separator_(0)
{
    setModal(true);
    createLayout();
}

TreeViewDialog::TreeViewDialog(QWidget *parent)
#ifndef Q_WS_QWS
: QDialog(0)
#else
: QDialog(0, Qt::FramelessWindowHint)
#endif
, close_button_(QIcon(":/images/close.png"), "", 0)
, tree_(0, 0)
, status_bar_(0, ui::PROGRESS|ui::MESSAGE)
, layout_(this)
, title_layout_(0)
, title_icon_(0)
, title_label_(0)
, hor_separator_(0)
{
    setModal(true);
    createLayout();
}

TreeViewDialog::~TreeViewDialog(void)
{
}

void TreeViewDialog::createLayout()
{
    setAutoFillBackground(true);
    setBackgroundRole(QPalette::Dark);

    layout_.setSpacing(0);
    layout_.setContentsMargins(0, 0, 0, 0);

    int SPACING = 2;
    title_layout_.setSpacing(SPACING);
    title_layout_.setContentsMargins(5, 0, 5, 0);
    title_icon_.setPixmap(QPixmap(":/images/tree_view.png"));
    QFont font(title_label_.font());
    font.setPointSize(20);
    title_label_.setFont(font);
    title_label_.setAlignment(Qt::AlignVCenter);
    title_label_.useTitleBarStyle();
    title_layout_.addWidget(&title_icon_, 10);
    title_layout_.addWidget(&title_label_, 500);
    title_layout_.addWidget(&close_button_, 50);
    close_button_.useDefaultHeight();
    close_button_.setFocusPolicy(Qt::NoFocus);
    layout_.addLayout(&title_layout_);

    hor_separator_.setFixedHeight(5);
    hor_separator_.setFrameShape(QFrame::HLine);
    hor_separator_.setAutoFillBackground(true);
    hor_separator_.setBackgroundRole(QPalette::Shadow);
    layout_.addWidget(&hor_separator_, 2);

    // tree widget.
    tree_.showHeader(false);
    layout_.addWidget(&tree_);

    // Status bar
    // TODO. Add support to Menu, Connection status, Power status and Stylus
    layout_.addWidget(&status_bar_);

    // Connections.
    connect(&close_button_, SIGNAL(clicked(bool)),
            this, SLOT(onCloseClicked(bool)), Qt::QueuedConnection);
    connect(&tree_, SIGNAL(activated(const QModelIndex &)),
            this, SLOT(onItemActivated(const QModelIndex &)));
    connect(&tree_, SIGNAL(positionChanged(int, int)),
            &status_bar_, SLOT(setProgress(int, int)));
    connect(&status_bar_, SIGNAL(progressClicked(const int, const int)),
            this, SLOT(onStatusBarClicked(const int, const int)));
}

void TreeViewDialog::setModel(QStandardItemModel * model)
{
    tree_.setModel(model);
}

int TreeViewDialog::popup(const QString &title,
                          const QModelIndex &index)
{
    selected_ = index;
    title_label_.setText(title);
    status_bar_.setProgress(tree_.currentPage(), tree_.pages());
    tree_.select(index);

    showMaximized();
    onyx::screen::instance().flush(this, onyx::screen::ScreenProxy::GC);
    return exec();
}

QModelIndex TreeViewDialog::selectedItem()
{
    return selected_;
}

void TreeViewDialog::setTitleIcon(const QString & path)
{
    title_icon_.setPixmap(QPixmap(path));
}

void TreeViewDialog::keyPressEvent(QKeyEvent * ke)
{
    ke->accept();
}

void TreeViewDialog::keyReleaseEvent(QKeyEvent *ke)
{
    ke->accept();
    switch (ke->key())
    {
    case Qt::Key_Escape:
        reject();
        break;
    case Qt::Key_PageDown:
    case Qt::Key_PageUp:
    case Qt::Key_Left:
    case Qt::Key_Right:
    case Qt::Key_Up:
    case Qt::Key_Down:
    case Qt::Key_Return:
        tree_.keyReleaseEvent(ke);
        break;
    default:
        QDialog::keyReleaseEvent(ke);
        break;
    }
}

void TreeViewDialog::onStatusBarClicked(const int percent,
                                        const int value)
{
    tree_.jumpToPage(value);
}

void TreeViewDialog::onItemActivated(const QModelIndex &index)
{
    selected_ = index;
    accept();
}

void TreeViewDialog::onCloseClicked(bool)
{
    reject();
}

bool TreeViewDialog::event(QEvent *event)
{
    bool ret = QDialog::event(event);
    if (event->type() == QEvent::UpdateRequest && onyx::screen::instance().isUpdateEnabled())
    {
        static int count = 0;
        qDebug("TreeViewDialog update %d", ++count);
        onyx::screen::instance().updateWidget(this, onyx::screen::ScreenProxy::GC);
    }
    return ret;
}

} // namespace ui
