
#include "onyx/wireless/connecting_dialog.h"
#include "onyx/screen/screen_proxy.h"
#include "onyx/sys/sys.h"
#include "onyx/data/network_types.h"

namespace ui
{

ConnectingDialog::ConnectingDialog(QWidget *parent, SysStatus & sys)
#ifndef Q_WS_QWS
    : QDialog(parent, 0)
#else
    : QDialog(parent, Qt::FramelessWindowHint)
#endif
    , content_layout_(this)
    , state_widget_(this)
    , sys_(sys)
{
    setAutoFillBackground(false);
    createLayout();
}

ConnectingDialog::~ConnectingDialog()
{
}

void ConnectingDialog::createLayout()
{
    const int SPACING = 2;
    content_layout_.setSizeConstraint(QLayout::SetMaximumSize);
    content_layout_.setSpacing(SPACING);
    content_layout_.setContentsMargins(SPACING, SPACING, SPACING, SPACING);

    const int MARGINS = 10;
    state_widget_.setWordWrap(true);
    state_widget_.setAlignment(Qt::AlignCenter);
    state_widget_.setContentsMargins(MARGINS, 0, MARGINS, 0);

    content_layout_.addWidget(&state_widget_,600);
    content_layout_.addStretch(0);


    QObject::connect(&sys_, SIGNAL(pppConnectionChanged(const QString &, int)),
                     this, SLOT(onPppConnectionChanged(const QString &, int)));


}

void ConnectingDialog::onPppConnectionChanged(const QString &message, int status)
{
    if (status == TG_CHECKING_NETWORK)
    {
        state_widget_.setText(tr("Searching Network..."));
    }
    else if (status == TG_CONNECTING)
    {
        state_widget_.setText(tr("Connecting..."));
    }
    else if (status == TG_CONNECTED)
    {
        QTimer::singleShot(1500, this, SLOT(accept()));
    }
    else if (status == TG_DISCONNECTED)
    {
        state_widget_.setText(tr("Disconnected."));
    }
}


int  ConnectingDialog::popup(bool show_profile)
{
    if (!sys_.isPowerSwitchOn())
    {
        state_widget_.setText(tr("3G Connection is off. Please turn 3G switch on."));
    }
    onyx::screen::instance().flush(0, onyx::screen::ScreenProxy::GC);

    return exec();
}

void ConnectingDialog::keyPressEvent(QKeyEvent *ke)
{
    if (ke->key() == Qt::Key_Escape)
    {
        ke->accept();
        accept();
    }
    QDialog::keyPressEvent(ke);
}

void ConnectingDialog::keyReleaseEvent(QKeyEvent *ke)
{
    ke->accept();
}

bool ConnectingDialog::event(QEvent * e)
{
    bool ret = QDialog::event(e);
    if (e->type() == QEvent::UpdateRequest)
    {
        onyx::screen::instance().updateWidget(this, onyx::screen::ScreenProxy::GU);
        e->accept();
        return true;
    }
    return ret;
}

void ConnectingDialog::onCloseClicked()
{
    reject();
}

}   // namespace ui

