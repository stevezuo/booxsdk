
#include <algorithm>
#include "onyx/ui/ui_global.h"
#include "onyx/ui/menu.h"
#include "onyx/ui/menu_item.h"
#include "onyx/ui/keyboard_navigator.h"
#include "onyx/screen/screen_proxy.h"



namespace ui
{

using namespace std;
static QTime popup_time;

const int MIN_ELAPSED = 800;
const int STATE_HIDE = 0;
const int STATE_INITIAL_VISIBLE = 1;
const int STATE_IGNORE = 2;
static int state = STATE_HIDE;



PopupMenu::PopupMenu(QWidget *parent, bool load_translator)
: QDialog(parent, Qt::Popup|Qt::FramelessWindowHint)
, selected_category_(0)
, exclusive_group_(this)
, system_actions_(0)
, big_layout_(this)
, menu_layout_(0)
, categroy_section_()
, children_section_()
, system_section_()
, separator_(this)
, system_separator_(this)
{
    createMenuLayout();
}

PopupMenu::~PopupMenu()
{
    clear();
}

// Create menu.
void PopupMenu::createMenuLayout()
{
    big_layout_.setContentsMargins(5, 5, 5, 5);
    big_layout_.setSpacing(2);

    big_layout_.addLayout(&menu_layout_);

    // Left category section.
    menu_layout_.addLayout(&categroy_section_.layout());
    menu_layout_.setSpacing(2);

    // Separator
    separator_.setFixedWidth(1);
    separator_.setFocusPolicy(Qt::NoFocus);
    separator_.setFrameShape(QFrame::VLine);
    separator_.setAutoFillBackground(true);
    separator_.setBackgroundRole(QPalette::Light);
    menu_layout_.addWidget(&separator_);

    // Childrent section.
    menu_layout_.addLayout(&children_section_.layout());

    // System action.
    system_separator_.setFocusPolicy(Qt::NoFocus);
    system_separator_.setFixedHeight(1);
    system_separator_.setFrameShape(QFrame::HLine);
    system_separator_.setAutoFillBackground(true);
    system_separator_.setBackgroundRole(QPalette::Light);

    big_layout_.addWidget(&system_separator_);
    big_layout_.addLayout(&system_section_.layout());

    // Setup connection.
    connect(&categroy_section_, SIGNAL(clicked(MenuItem *, QAction *)),
            this, SLOT(onGroupClicked(MenuItem *, QAction *)));
    connect(&children_section_, SIGNAL(clicked(MenuItem *, QAction *)),
            this, SLOT(onItemClicked(MenuItem *, QAction *)));
    connect(&system_section_, SIGNAL(clicked(MenuItem *, QAction *)),
            this, SLOT(onSystemActionClicked(MenuItem *, QAction *)));
}

void PopupMenu::addCategory(QAction *category)
{
    categroy_section_.addItem(this, category, categroy_section_.items().size(), 0);
    if (category->isChecked())
    {
        selected_category_ = category;
    }
}

/// Arrange items belong to the group. Before using this function, caller
/// should make sure the group has been correctly initialized.
/// \param items The items belong to the group.
/// \return This function returns true when one item in the group is selected.
/// Otherwise it returns false.
bool PopupMenu::arrangeItems(BaseActions *items)
{
    // Arrange all children items for this group.
    return children_section_.arrangeItems(this, items, rows(), columns());
}

int PopupMenu::rows()
{
    return 5;
}

int  PopupMenu::columns()
{
    return 5;
}

void PopupMenu::keyPressEvent(QKeyEvent *ke)
{
    ke->accept();
}

void PopupMenu::keyReleaseEvent(QKeyEvent *ke)
{
    // Check the current selected type.
    ke->accept();

    if (popup_time.elapsed() < MIN_ELAPSED)
    {
        return;
    }

    switch (ke->key())
    {
    case Qt::Key_Left:
    case Qt::Key_Right:
    case Qt::Key_Up:
    case Qt::Key_Down:
    case Qt::Key_PageDown:
    case Qt::Key_PageUp:
        navigate(ke->key());
        break;
    case Qt::Key_Return:
        activate();
        break;
    case ui::Device_Menu_Key:
    case Qt::Key_Escape:
    case Qt::Key_Home:
        reject();
        break;
    }
}

/// Navigate according to the key.
void PopupMenu::navigate(int key)
{
    int ret = -1;
    QWidget * wnd = ui::moveFocus(this, key);
    if (wnd)
    {
        wnd->setFocus();
        ret = 1;
    }

    if (ret > 0)
    {
        onyx::screen::instance().enableUpdate(false);
        repaint();
        onyx::screen::instance().enableUpdate(true);
        onyx::screen::instance().updateWidget(this, onyx::screen::ScreenProxy::DW, false);
    }
}

void PopupMenu::activate()
{
    if (categroy_section_.hasFocus())
    {
        categroy_section_.activate();
    }
    else if (children_section_.hasFocus())
    {
        children_section_.activate();
    }
    else if (system_section_.hasFocus())
    {
        system_section_.activate();
    }
}

void PopupMenu::paintEvent(QPaintEvent *pe)
{
    QPainter p(this);
    QRect rc = rect().adjusted(1, 1, -1, -1);
    {
        QPainterPath path;
        path.addRoundedRect(rc, 10, 10);
        QPen pen(Qt::SolidLine);
        pen.setColor(Qt::black);
        pen.setWidth(2);
        p.setPen(pen);
        p.drawPath(path);
    }

    {
        /*
        rc = rc.adjusted(1, 1, -1, -1);
        QPainterPath path;
        path.addRoundedRect(rc, 10, 10);
        QPen pen(Qt::SolidLine);
        pen.setColor(QColor(255, 255, 255, 255));
        pen.setWidth(2);
        p.setPen(pen);
        p.drawPath(path);
        */
    }

    rc = rc.adjusted(1, 1, -1, -1);
    QPainterPath path;
    path.addRoundedRect(rc, 10, 10);
    /*
    QLinearGradient fade(0, 0, 0, rc.height());
    fade.setColorAt(0, QColor(0, 0, 0, 255));
    fade.setColorAt(0.01, QColor(128, 128, 128, 255));
    fade.setColorAt(0.02, QColor(20, 20, 20, 255));
    fade.setColorAt(0.98, QColor(20, 20, 20, 255));
    fade.setColorAt(0.99, QColor(128, 128, 128, 255));
    fade.setColorAt(1.0,  QColor(0, 0, 0, 255));
    p.fillPath(path, fade);
    */
    p.fillPath(path, QBrush(QColor(255, 255, 255, 255)));
}

void PopupMenu::resizeEvent(QResizeEvent *)
{
    QPainterPath p;
    p.addRoundedRect(rect(), 10, 10);
    QRegion maskedRegion(p.toFillPolygon().toPolygon());
    setMask(maskedRegion);
}

void PopupMenu::mousePressEvent(QMouseEvent *me)
{
    me->accept();
}

void PopupMenu::mouseReleaseEvent(QMouseEvent *me)
{
    if (popup_time.elapsed() < MIN_ELAPSED)
    {
        return;
    }

    if (!rect().contains(me->pos()))
    {
        me->accept();
        reject();
        return;
    }
    return QDialog::mouseReleaseEvent(me);
}

void PopupMenu::showEvent(QShowEvent * event)
{
    QDialog::showEvent(event);
    state = STATE_INITIAL_VISIBLE;
}

QAction * PopupMenu::selectedCategory()
{
    if (selected_category_ == 0 && categroy_section_.items().size() > 0)
    {
        return categroy_section_.items().front()->action();
    }
    return selected_category_;
}

void PopupMenu::updateClickedItem(QWidget *wnd)
{
    onyx::screen::instance().enableUpdate(false);
    repaint();
    onyx::screen::instance().enableUpdate(true);
    onyx::screen::instance().updateWidget(
        this,
        onyx::screen::ScreenProxy::DW,
        false,
        onyx::screen::ScreenCommand::WAIT_ALL);
}

void PopupMenu::onItemClicked(MenuItem* wnd, QAction *action)
{
    // Make it selected and update the user interface.
    updateClickedItem(wnd);
    accept();
}

void PopupMenu::onGroupClicked(MenuItem* wnd, QAction *action)
{
    onyx::screen::instance().enableUpdate(true);
    selected_category_ = action;
    arrangeItems(all_actions_[action]);
}

/// We are able to handle some system action directly in
/// popup menu.
void PopupMenu::onSystemActionClicked(MenuItem* wnd, QAction *action)
{
    // Check which one has been selected.
    selected_category_ = system_actions_->category();
    updateClickedItem(wnd);
    accept();
}

/// Add a group to the popup menu.
void PopupMenu::addGroup(BaseActions * actions)
{
    all_actions_[actions->category()] = actions;
    actions->category()->setActionGroup(&exclusive_group_);
    addCategory(actions->category());
}

/// Set the system action. There is only one system action.
void PopupMenu::setSystemAction(BaseActions * actions)
{
    system_actions_ = actions;
    system_section_.arrangeItems(this, actions, 1, 6);
}

int PopupMenu::popup(const QString &)
{
    // Reset visible.
    state = STATE_HIDE;

    QAction *group = selectedCategory();
    QGridLayout & layout = categroy_section_.layout();
    layout.setRowStretch(layout.rowCount(), 100);

    // Disable the screen update now. Most applications will
    // update the screen when popup menu is shown. The reason is
    // that Qt system asks these applications to paint themselves
    // when menu is shown. The Qt framebuffer is updated but it's not necessary
    // to update the screen here.
    onyx::screen::instance().enableUpdate(false);

    arrangeItems(all_actions_[group]);
    setModal(true);
    // show();

    // Record current time to avoid the flick.
    // grabMouse();
    popup_time.start();

    // Ensure all pending events has been processed.
    // Use pre-defined timer to prevent deadlock.
    QTime t;
    t.start();
    while (QCoreApplication::hasPendingEvents() && t.elapsed() < 1500)
    {
        QCoreApplication::processEvents();
    }
    QCoreApplication::removePostedEvents(0);

    int ret = exec();

    // releaseMouse();
    onyx::screen::instance().flush(3000);

    if (ret == QDialog::Rejected)
    {
        // Post process.
        onyx::screen::instance().flush(
            0,
            onyx::screen::ScreenProxy::GC,
            true,
            onyx::screen::ScreenCommand::WAIT_ALL);
    }
    return ret;
}

void PopupMenu::clear()
{
    categroy_section_.clear();
    children_section_.clear();
    system_section_.clear();
}

bool PopupMenu::loadTranslator()
{
    // Not sure it will cause memory leak.
    static QTranslator *pre_translator;
    QTranslator *translator = new QTranslator;
    QString file = "onyx_ui_" + QLocale::system().name() + ".qm";

    // Double check.
    QDir dir(QDir::home());
    QString path = SHARE_ROOT;
    if (!path.isEmpty())
    {
        dir.cd(path);
    }

    if (!dir.cd("translations"))
    {
        return false;
    }

    qDebug("Locale file %s", qPrintable(dir.absoluteFilePath(file)));
    translator->load(dir.absoluteFilePath(file));

    if (pre_translator)
    {
        QApplication::removeTranslator(pre_translator);
    }
    QApplication::installTranslator(translator);
    pre_translator = translator;
    return true;
}

bool PopupMenu::event(QEvent * e)
{
    bool ret = QDialog::event(e);
    if (e->type() == QEvent::UpdateRequest)
    {
        if (state == STATE_INITIAL_VISIBLE)
        {
            onyx::screen::instance().flush(
                this,
                onyx::screen::ScreenProxy::GC,
                false,
                onyx::screen::ScreenCommand::WAIT_ALL);
            state = STATE_IGNORE;
            onyx::screen::instance().enableUpdate(false);
        }
        else if (state == STATE_IGNORE)
        {
            onyx::screen::instance().updateWidget(this, onyx::screen::ScreenProxy::GU);
        }
    }
    return ret;
}

}
