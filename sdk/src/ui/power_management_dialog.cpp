
#include "onyx/screen/screen_proxy.h"
#include "onyx/ui/power_management_dialog.h"
#include "onyx/ui/keyboard_navigator.h"

namespace ui
{

static const char* SCOPE = "pm";
struct Interval
{
    const char * title;
    int seconds;
};

/// Define all standby items.
static const Interval STANDBY_ITEMS[] =
{
    {QT_TRANSLATE_NOOP("pm","3 Minutes"), 180 * 1000},
    {QT_TRANSLATE_NOOP("pm","15 Minutes"), 900 * 1000},
    {QT_TRANSLATE_NOOP("pm","30 Minutes"), 1800 * 1000},
    {QT_TRANSLATE_NOOP("pm","Never"), 0},
};
static const int STANDBY_COUNT = sizeof(STANDBY_ITEMS) / sizeof(STANDBY_ITEMS[0]);

/// Define all shutdown items.
static const Interval SHUTDOWN_ITEMS[] =
{
    {QT_TRANSLATE_NOOP("pm","5 Minutes"), 300 * 1000},
    {QT_TRANSLATE_NOOP("pm","15 Minutes"), 900 * 1000},
    {QT_TRANSLATE_NOOP("pm","30 Minutes"), 1800 * 1000},
    {QT_TRANSLATE_NOOP("pm","Never"), 0},
};
static const int SHUTDOWN_COUNT = sizeof(SHUTDOWN_ITEMS) / sizeof(SHUTDOWN_ITEMS[0]);

PowerManagementDialog::PowerManagementDialog(QWidget *parent, SysStatus & ref)
    : OnyxDialog(parent)
    , status_(ref)
    , ver_layout_(&content_widget_)
    , standby_layout_(0)
    , standby_group_(0)
    , shutdown_layout_(0)
    , shutdown_group_(0)
    , hor_layout_(0)
    , ok_(QApplication::tr("OK"), 0)
    , sys_standby_interval_(0)
    , standby_interval_(0)
    , sys_shutdown_interval_(0)
    , shutdown_interval_(0)
{
    setModal(true);
    resize(400, 540);
    createLayout();
}

PowerManagementDialog::~PowerManagementDialog(void)
{
}

int PowerManagementDialog::exec()
{
    shadows_.show(true);
    show();
    onyx::screen::instance().flush();
    onyx::screen::instance().updateWidgetRegion(
        0,
        outbounding(parentWidget()),
        onyx::screen::ScreenProxy::GC,
        false,
        onyx::screen::ScreenCommand::WAIT_ALL);
    return QDialog::exec();
}

void PowerManagementDialog::keyPressEvent(QKeyEvent *ke)
{
    ke->accept();
}

void PowerManagementDialog::keyReleaseEvent(QKeyEvent *ke)
{
    QWidget *wnd = 0;
    ke->accept();
    switch (ke->key())
    {
    case Qt::Key_Up:
    case Qt::Key_Left:
    case Qt::Key_Right:
    case Qt::Key_Down:
    case Qt::Key_PageDown:
    case Qt::Key_PageUp:
        wnd = ui::moveFocus(&content_widget_, ke->key());
        if (wnd)
        {
            wnd->setFocus();
        }
        break;
    case Qt::Key_Return:
        break;
    case Qt::Key_Escape:
        reject();
        break;
    }
}

void PowerManagementDialog::createLayout()
{
    content_widget_.setBackgroundRole(QPalette::Button);

    // Retrieve the values from system status.
    sys_standby_interval_ = status_.suspendInterval();
    standby_interval_ = sys_standby_interval_;

    sys_shutdown_interval_ = status_.shutdownInterval();
    shutdown_interval_ = sys_shutdown_interval_;

    updateTitle(QApplication::tr("Power Management"));
    updateTitleIcon(QPixmap(":/images/power.png"));

    // The big layout.
    ver_layout_.setContentsMargins(SPACING, 0, SPACING, 0);
    ver_layout_.addSpacing(10);

    // Standby layout
    // Label.
    standby_image_label_.setPixmap(QPixmap(":/images/standby.png"));
    standby_text_label_.setText(QApplication::tr("Time to standby"));
    standby_hor_layout_.setContentsMargins(10, 0, 0, 0);
    standby_hor_layout_.addWidget(&standby_image_label_);
    standby_hor_layout_.addWidget(&standby_text_label_);
    standby_hor_layout_.addStretch(0);
    standby_layout_.addLayout(&standby_hor_layout_);
    standby_layout_.addStretch(0);

    OnyxCheckBox * btn = 0;
    int index = 0;
    bool set_intial_focus = true;
    for(int row = 0; row < STANDBY_COUNT; ++row, ++index)
    {
        btn = new OnyxCheckBox(qApp->translate(SCOPE, STANDBY_ITEMS[index].title), 0);
        standby_group_.addButton(btn);
        standby_buttons_.push_back(btn);
        if (sys_standby_interval_ == STANDBY_ITEMS[index].seconds)
        {
            btn->setFocus();
            btn->setChecked(true);
            set_intial_focus = false;
        }
        connect(btn, SIGNAL(clicked(bool)), this, SLOT(onStandbyButtonChanged(bool)), Qt::QueuedConnection);
        standby_layout_.addWidget(btn);
    }

    if (set_intial_focus)
    {
        standby_buttons_[0]->setFocus();
    }

    ver_layout_.addLayout(&standby_layout_);

    // Shutdown

    // Label
    shutdown_image_label_.setPixmap(QPixmap(":/images/shutdown.png"));
    shutdown_text_label_.setText(QApplication::tr("Time to shutdown"));
    shutdown_hor_layout_.setContentsMargins(10, 0, 0, 0);
    shutdown_hor_layout_.addWidget(&shutdown_image_label_);
    shutdown_hor_layout_.addWidget(&shutdown_text_label_);
    shutdown_hor_layout_.addStretch(0);
    shutdown_layout_.addLayout(&shutdown_hor_layout_);
    shutdown_layout_.addStretch(0);

    index = 0;
    for(int row = 0; row < SHUTDOWN_COUNT; ++row, ++index)
    {
        btn = new OnyxCheckBox(qApp->translate(SCOPE, SHUTDOWN_ITEMS[index].title), 0);
        shutdown_group_.addButton(btn);
        shutdown_buttons_.push_back(btn);
        if (sys_shutdown_interval_ == SHUTDOWN_ITEMS[index].seconds)
        {
            btn->setFocus();
            btn->setChecked(true);
        }
        connect(btn, SIGNAL(clicked(bool)), this, SLOT(onShutdownButtonChanged(bool)), Qt::QueuedConnection);
        shutdown_layout_.addWidget(btn);
    }
    ver_layout_.addLayout(&shutdown_layout_);

    // OK cancel buttons.
    connect(&ok_, SIGNAL(clicked(bool)), this, SLOT(onOkClicked(bool)));

    ok_.useDefaultHeight();
    ok_.setCheckable(false);
    ok_.setFocusPolicy(Qt::TabFocus);
    hor_layout_.addStretch(0);
    hor_layout_.addWidget(&ok_);

    ver_layout_.addStretch(0);
    ver_layout_.addLayout(&hor_layout_);
}

void PowerManagementDialog::onStandbyButtonChanged(bool state)
{
    int count = static_cast<int>(standby_buttons_.size());
    int i = 0;
    for(; i < count; ++i)
    {
        if (standby_buttons_[i]->isChecked())
        {
            standby_interval_ = STANDBY_ITEMS[i].seconds;
            break;
        }
    }

    // Never shutdown.
    int j = shutdown_buttons_.size() - 1;
    shutdown_buttons_[j]->setChecked(true);
    shutdown_interval_ = SHUTDOWN_ITEMS[j].seconds;
}

void PowerManagementDialog::onShutdownButtonChanged(bool state)
{
    int count = static_cast<int>(shutdown_buttons_.size());
    int i = 0;
    for(; i < count; ++i)
    {
        if (shutdown_buttons_[i]->isChecked())
        {
            shutdown_interval_ = SHUTDOWN_ITEMS[i].seconds;
            break;
        }
    }

    // Never deep sleep.
    int j = standby_buttons_.size() - 1;
    standby_buttons_[j]->setChecked(true);
    standby_interval_ = STANDBY_ITEMS[j].seconds;
}

bool PowerManagementDialog::event(QEvent* qe)
{
    bool ret = QDialog::event(qe);
    if (qe->type() == QEvent::UpdateRequest)
    {
        onyx::screen::instance().sync(&shadows_.hor_shadow());
        onyx::screen::instance().sync(&shadows_.ver_shadow());
        onyx::screen::instance().updateWidget(this, onyx::screen::ScreenProxy::GU);
    }
    return ret;
}

void PowerManagementDialog::onOkClicked(bool)
{
    if (standby_interval_ != sys_standby_interval_)
    {
        status_.setSuspendInterval(standby_interval_);
    }

    if (shutdown_interval_ != sys_shutdown_interval_ )
    {
        status_.setShutdownInterval(shutdown_interval_);
    }

    accept();
    onyx::screen::instance().flush(this, onyx::screen::ScreenProxy::GU);
}

}   // namespace ui

