
#include "onyx/ui/message_dialog.h"
#include "onyx/ui/keyboard_navigator.h"
#include "onyx/screen/screen_proxy.h"

namespace ui
{

static const int ICON_WIDTH = 32;

MessageDialog::MessageDialog(QMessageBox::Icon icon,
                             const QString & title,
                             const QString & text,
                             QMessageBox::StandardButtons buttons,
                             QWidget * parent)
    : OnyxDialog(parent)
    , icon_type_(icon)
    , buttons_(buttons)
    , content_vbox_(&content_widget_)
    , info_hbox_(0)
    , button_hbox_(0)
    , info_text_(text, this)
{
    // Dialog attribute.
    setModal(true);

    int width = 0;
    if (parent)
    {
        width = parent->size().width();
    }
    else
    {
        width = QApplication::desktop()->size().width();
    }

    createLayout();
    updateTitle(title);
    setFixedWidth(width * 4 / 5);
}

MessageDialog::~MessageDialog()
{
}

int MessageDialog::exec()
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

void MessageDialog::createLayout()
{
    // title hbox.
    title_icon_label_.setPixmap(pixmap(icon_type_));

    // content layout.
    content_vbox_.addLayout(&info_hbox_);
    content_vbox_.addLayout(&button_hbox_);

    // info box.
    info_hbox_.setContentsMargins(0, 0, 0, 0);
    info_hbox_.setSpacing(0);
    info_hbox_.addWidget(&info_text_);
    info_text_.useMessageStyle(width());

    button_hbox_.setContentsMargins(SPACING, SPACING, SPACING, SPACING);
    button_hbox_.setSpacing(SPACING * 2);
    button_hbox_.addStretch(0);
    createButtons(buttons_);
}

void MessageDialog::createButtons(QMessageBox::StandardButtons buttons)
{
    OnyxPushButton * button = 0;
    // TODO, use table driven later
    if (buttons & QMessageBox::Ok)
    {
        button = new OnyxPushButton(tr("OK"), this);
        button->setData(QMessageBox::Ok);
        addButton(button);
    }
    if (buttons & QMessageBox::Cancel)
    {
        button = new OnyxPushButton(tr("Cancel"), this);
        button->setData(QMessageBox::Cancel);
        addButton(button);
    }
    if (buttons & QMessageBox::Yes)
    {
        button = new OnyxPushButton(tr("Yes"), this);
        button->setData(QMessageBox::Yes);
        addButton(button);
    }
    if (buttons & QMessageBox::No)
    {
        button = new OnyxPushButton(tr("No"), this);
        button->setData(QMessageBox::No);
        addButton(button);
    }
}

void MessageDialog::addButton(OnyxPushButton *button)
{
    connect(button, SIGNAL(clicked(bool)), this, SLOT(onButtonClicked()), Qt::QueuedConnection);
    button->setFocusPolicy(Qt::TabFocus);
    button_list_.push_back(button);
    button_hbox_.addWidget(button);
}

void MessageDialog::navigate(int key)
{
    QWidget * wnd = ui::moveFocus(&content_widget_, key);
    if (wnd)
    {
        wnd->setFocus();
    }
}

QPixmap MessageDialog::pixmap(QMessageBox::Icon icon)
{
    switch (icon)
    {
        case QMessageBox::NoIcon:
        case QMessageBox::Information:
            return QPixmap(":/images/information.png");
        case QMessageBox::Warning:
            return QPixmap(":/images/warning.png");
        case QMessageBox::Critical:
            return QPixmap(":/images/critical.png");
        case QMessageBox::Question:
            return QPixmap(":/images/question.png");
        default:
            return QPixmap(":/images/information.png");
    }
}

void MessageDialog::updateInformation(const QString & text)
{
    info_text_.setText(text);
    info_text_.useMessageStyle(width());
}

void MessageDialog::keyPressEvent(QKeyEvent *key)
{
    key->accept();
}

void MessageDialog::keyReleaseEvent(QKeyEvent *key)
{
    // Check the current selected type.
    key->accept();
    switch (key->key())
    {
    case Qt::Key_Left:
    case Qt::Key_Right:
    case Qt::Key_Up:
    case Qt::Key_Down:
    case Qt::Key_PageDown:
    case Qt::Key_PageUp:
        navigate(key->key());
        break;
    case Qt::Key_Escape:
        reject();
        break;
    }
}

void MessageDialog::resizeEvent(QResizeEvent *e)
{
    // info_text_.useMessageStyle(width());
    OnyxDialog::resizeEvent(e);
}

OnyxPushButton *MessageDialog::button(int button_id)
{
    foreach(OnyxPushButton * button, button_list_)
    {
        if (button->data().toInt() == button_id)
        {
            return button;
        }
    }
    return 0;
}

void MessageDialog::onButtonClicked()
{
    QObject * object = sender();
    if (object == 0)
    {
        return;
    }

    int value = static_cast<OnyxPushButton *>(object)->data().toInt();
    done(value);
}

bool MessageDialog::event(QEvent * e)
{
    bool ret = OnyxDialog::event(e);
    if (e->type() == QEvent::UpdateRequest)
    {
        onyx::screen::instance().updateWidget(
            this,
            onyx::screen::ScreenProxy::GU,
            true,
            onyx::screen::ScreenCommand::WAIT_ALL);
        e->accept();
        return true;
    }
    return ret;
}


}
