#include "onyx/screen/screen_proxy.h"
#include "onyx/ui/main_window.h"

using namespace ui;
using namespace base;

namespace vbf
{

MainWindow::MainWindow(QObject *parent)
#ifndef Q_WS_QWS
    : QWidget(0)
#else
    : QWidget(0, Qt::FramelessWindowHint)
#endif
    , active_view_(0)
    , model_(0)
    , vlayout_(this)
    , hlayout_(0)
    , status_bar_(this, MENU | PROGRESS | MESSAGE | STYLUS | BATTERY | CLOCK | VOLUME | SCREEN_REFRESH)
    , mandatory_update_(false)
{
    setAutoFillBackground(true);
    setBackgroundRole(QPalette::Base);

#ifndef Q_WS_QWS
    resize(600, 800);
#else
    resize(qApp->desktop()->screenGeometry().size());
#endif

    initializeLayout();

    // connect the base signals
    connect(this,
            SIGNAL(createView(int, MainWindow*, QWidget*&)),
            parent,
            SLOT(onCreateView(int, MainWindow*, QWidget*&)));

    connect(this,
            SIGNAL(attachView(int, QWidget*, MainWindow*)),
            parent,
            SLOT(onAttachView(int, QWidget*, MainWindow*)));

    connect(this,
            SIGNAL(deattachView(int, QWidget*, MainWindow*)),
            parent,
            SLOT(onDeattachView(int, QWidget*, MainWindow*)));

    // connect the signals with member widgets
    connect(this,
            SIGNAL(itemStatusChanged(const StatusBarItemType, const int)),
            &status_bar_,
            SLOT(setItemState(const StatusBarItemType, const int)));

    connect(&status_bar_,
            SIGNAL(menuClicked()),
            this,
            SLOT(onPopupContextMenu()));

    connect(&status_bar_,
            SIGNAL(progressClicked(const int, const int)),
            this,
            SLOT(onPagebarClicked(const int, const int)));

    // connect the signal of system status
    SysStatus & sys_status = sys::SysStatus::instance();
    connect(&sys_status,
            SIGNAL(musicPlayerStateChanged(int)),
            this,
            SLOT(onMusicPlayerStateChanged(int)));
}

MainWindow::~MainWindow()
{
    clearViews();
}

void MainWindow::attachModel(BaseModel* model)
{
    model_ = model;
}

void MainWindow::setContentMargins(int left, int top, int right, int bottom)
{
    hlayout_.setContentsMargins(left, top, right, bottom);
}

QWidget* MainWindow::getView(int type)
{
    ViewIter iter = views_.find(type);
    if (iter == views_.end())
    {
        // create a new view
        QWidget* new_view = 0;
        emit createView(type, this, new_view);
        if (new_view == 0)
        {
            qDebug("Cannot create view:%d", type);
            return 0;
        }

        views_[type] = new_view;
        new_view->hide();
        hlayout_.insertWidget(0, new_view);
        return new_view;
    }

    return iter.value();
}

QWidget* MainWindow::activateView(int type)
{
    QWidget* negative_view = active_view_;
    ViewIter iter = views_.find(type);
    if (iter == views_.end())
    {
        // create a new view
        QWidget* new_view = 0;
        emit createView(type, this, new_view);
        if (new_view == 0)
        {
            qDebug("Cannot create view:%d", type);
            return 0;
        }

        active_view_ = new_view;
        views_[type] = new_view;
        hlayout_.insertWidget(0, new_view);
    }
    else
    {
        active_view_ = iter.value();
    }

    if (negative_view != 0)
    {
        // deactivate the previous view
        int negative_type = views_.key(negative_view);
        emit deattachView(negative_type, negative_view, this);
        negative_view->hide();
    }

    // activate this view
    emit attachView(type, active_view_, this);
    active_view_->show();
    active_view_->setFocus();
    return active_view_;
}

void MainWindow::handlePositionChanged(const int current, const int total)
{
    status_bar_.setProgress(current + 1, total);
}

void MainWindow::handleSetStatusBarFunctions(const StatusBarItemTypes funcs)
{
    status_bar_.addItems(funcs);
}

void MainWindow::handleFullScreen(bool enable)
{
    if (enable)
    {
        status_bar_.hide();
    }
    else
    {
        status_bar_.show();
    }
}

void MainWindow::handleClockClicked()
{
    status_bar_.onClockClicked();
}

void MainWindow::onPagebarClicked(const int percent, const int value)
{
    emit pagebarClicked(percent, value - 1);
}

void MainWindow::onPopupContextMenu()
{
    emit popupContextMenu();
}

void MainWindow::handleItemStatusChanged(const StatusBarItemType type,
                                         const int state)
{
    emit itemStatusChanged(type, state);
}

void MainWindow::onMusicPlayerStateChanged(int state)
{
    if (state == sys::HIDE_PLAYER || state == sys::STOP_PLAYER)
    {
        onyx::screen::instance().flush(this, onyx::screen::ScreenProxy::GC);
    }
}

void MainWindow::handleRequestUpdate(bool update_now)
{
    if (update_now)
    {
        qDebug("Update Screen Now");
        onyx::screen::instance().flush(this, onyx::screen::ScreenProxy::GU);
    }
    else
    {
        mandatory_update_ = true;
    }
}

void MainWindow::handlePopupJumpPageDialog()
{
    status_bar_.onMessageAreaClicked();
}

void MainWindow::initializeLayout()
{
    vlayout_.setSpacing(0);
    vlayout_.setContentsMargins(1, 0, 1, 0);

    hlayout_.setContentsMargins(0, 0, 0, 0);
    vlayout_.addLayout(&hlayout_);
    vlayout_.addWidget(&status_bar_);
}

void MainWindow::clearViews()
{
    ViewIter iter = views_.begin();
    for (; iter != views_.end(); iter++)
    {
        delete iter.value();
    }
    views_.clear();
}

bool MainWindow::event(QEvent * event)
{
    bool ret = QWidget::event(event);
    //qDebug("main window event type %d", event->type());
    if (event->type() == QEvent::UpdateRequest && onyx::screen::instance().isUpdateEnabled() &&
        (isActiveWindow() || mandatory_update_))
    {
        static int count = 0;
        if (mandatory_update_)
        {
            qDebug("Update request %d, Default", ++count);
            onyx::screen::instance().updateWidget(this);
        }
        else
        {
            qDebug("Update request %d, GU", ++count);
            onyx::screen::instance().updateWidget(this, onyx::screen::ScreenProxy::GU);
        }

        mandatory_update_ = false;
        event->accept();
        return true;
    }
    return ret;
}

}
