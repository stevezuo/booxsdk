#include "onyx/screen/screen_proxy.h"
#include "onyx/ui/password_dialog.h"
#include "onyx/ui/keyboard_navigator.h"

namespace ui
{

PasswordDialog::PasswordDialog(QWidget *parent)
    : OnyxDialog(parent)
    , hbox_(&content_widget_)
    , text_edit_("", this)
    , ok_button_(tr("OK"), this)
    , clear_button_(tr("Clear"), this)
    , show_plain_text_(tr("Show Plain Text"), this)
    , keyboard_(0)
{
    createLayout();

    // Widget attributes.
    setModal(true);
    setFocusPolicy(Qt::NoFocus);

    updateTitle(tr("Input Password"));
}

PasswordDialog::~PasswordDialog(void)
{
}

void PasswordDialog::createLayout()
{
    title_icon_label_.setPixmap(QPixmap(":/images/dictionary_search.png"));
    content_widget_.setFixedHeight(WIDGET_HEIGHT + 4 * SPACING);

    // hbox to layout line edit and buttons.
    hbox_.setContentsMargins(SPACING, 0, SPACING, 0);
    hbox_.setSpacing(SPACING * 4);

    // Line edit.
    text_edit_.setFixedHeight(WIDGET_HEIGHT);
    hbox_.addWidget(&text_edit_, 400);
    hbox_.addWidget(&ok_button_, 100);
    hbox_.addWidget(&clear_button_, 100);

    // Buttons.
    ok_button_.useDefaultHeight();
    show_plain_text_.setFixedHeight(WIDGET_HEIGHT);
    show_plain_text_.selectOnClicked(false);
    clear_button_.useDefaultHeight();

    show_plain_text_.setFocusPolicy(Qt::NoFocus);
    ok_button_.setFocusPolicy(Qt::NoFocus);
    show_plain_text_.selectOnClicked(false);
    clear_button_.setFocusPolicy(Qt::NoFocus);

    vbox_.addWidget(&show_plain_text_);

    // keyboard.
    keyboard_.attachReceiver(this);
    vbox_.addWidget(&keyboard_);

    // Setup connections.
    connect(&show_plain_text_, SIGNAL(clicked()), this, SLOT(onShowPlainTextClicked()), Qt::QueuedConnection);
    connect(&ok_button_, SIGNAL(clicked()), this, SLOT(onOKClicked()));
    connect(&clear_button_, SIGNAL(clicked()), &text_edit_, SLOT(clear()));
    connect(&timer_, SIGNAL(timeout()), this, SLOT(onTimeout()));
    connect(&text_edit_, SIGNAL(textChanged(const QString&)), this, SLOT(onTextChanged(const QString&)));

    // Install event filter
    show_plain_text_.installEventFilter(this);
    ok_button_.installEventFilter(this);
    clear_button_.installEventFilter(this);
    text_edit_.installEventFilter(this);
    keyboard_.installEventFilter(this);

    onShowPlainTextClicked();
}


void PasswordDialog::onShowPlainTextClicked()
{
    if (!show_plain_text_.isChecked())
    {
        text_edit_.setEchoMode(QLineEdit::Password);
    }
    else
    {
        text_edit_.setEchoMode(QLineEdit::Normal);
    }
}

void PasswordDialog::onOKClicked()
{
    shadows_.show(false);
    done(QDialog::Accepted);
}

void PasswordDialog::onCloseClicked()
{
    shadows_.show(false);
    done(QDialog::Rejected);
}

bool PasswordDialog::eventFilter(QObject *obj, QEvent *event)
{
    QWidget *wnd = 0;
    if (event->type() == QEvent::KeyRelease)
    {
        QKeyEvent *key_event = static_cast<QKeyEvent *>(event);
        if (obj == &text_edit_)
        {
            if (key_event->key() == Qt::Key_Down)
            {
                show_plain_text_.setFocus();
                return true;
            }
            else if (key_event->key() == Qt::Key_Right && ok_button_.isEnabled())
            {
                ok_button_.setFocus();
                return true;
            }
        }
        else if (obj == &ok_button_)
        {
            if (key_event->key() == Qt::Key_Right)
            {
                clear_button_.setFocus();
                return true;
            }
            else if (key_event->key() == Qt::Key_Down)
            {
                show_plain_text_.setFocus();
                return true;
            }
            else if (key_event->key() == Qt::Key_Left)
            {
                text_edit_.setFocus();
                return true;
            }
        }
        else if (obj == &clear_button_)
        {
            if (key_event->key() == Qt::Key_Down)
            {
                show_plain_text_.setFocus();
                return true;
            }
            else if (key_event->key() == Qt::Key_Left)
            {
                text_edit_.setFocus();
                return true;
            }
        }
        else if (obj == &keyboard_ || obj == &show_plain_text_)
        {
            if (key_event->key() == Qt::Key_Down ||
                key_event->key() == Qt::Key_Up ||
                key_event->key() == Qt::Key_Left ||
                key_event->key() == Qt::Key_Right)
            {
                wnd = moveFocus(this, key_event->key());
                if (wnd)
                {
                    wnd->setFocus();
                }
                return true;
            }
        }
    }
    // standard event processing
    return QObject::eventFilter(obj, event);
}

void PasswordDialog::onTextChanged(const QString& text)
{
    ok_button_.setEnabled(!text.isEmpty());
    clear_button_.setEnabled(!text.isEmpty());
}

/// This function is called by parent widget to display the search widget.
int PasswordDialog::popup(const QString &password)
{
    shadows_.show(true);
    if (isHidden())
    {
        show();
    }
    text_edit_.setText(password);

    QRect parent_rect = parentWidget()->rect();
    int border = Shadows::PIXELS;
    int width = parent_rect.width() - border * 2;
    if (size().width() != width)
    {
        resize(width, height());
    }

    int x = Shadows::PIXELS;
    int y = parentWidget()->height() - height() - Shadows::PIXELS;
    move(x, y);

    return exec();
}

QString PasswordDialog::password()
{
    return text_edit_.text();
}

void PasswordDialog::mouseMoveEvent(QMouseEvent *me)
{
    me->accept();
}

void PasswordDialog::mousePressEvent(QMouseEvent *me)
{
    me->accept();
}

void PasswordDialog::mouseReleaseEvent(QMouseEvent *me)
{
    me->accept();
}

void PasswordDialog::keyReleaseEvent(QKeyEvent *ke)
{
    int key = ke->key();
    if (key == Qt::Key_Escape || key == ui::Device_Menu_Key)
    {
        onCloseClicked();
        return;
    }
}

/// The keyPressEvent could be sent from virtual keyboard.
void PasswordDialog::keyPressEvent(QKeyEvent * ke)
{
    ke->accept();
    if (ke->key() == Qt::Key_Enter)
    {
        return;
    }
    else if (ke->key() == Qt::Key_Shift || ke->key() == Qt::Key_CapsLock)
    {
        return;
    }

    // Disable the parent widget to update screen.
    onyx::screen::instance().enableUpdate(false);

    if (text_edit_.hasFocus() ||
        (ke->key() != Qt::Key_Down &&
         ke->key() != Qt::Key_Up &&
         ke->key() != Qt::Key_Left &&
         ke->key() != Qt::Key_Right))
    {
        QKeyEvent * key_event = new QKeyEvent(ke->type(), ke->key(), ke->modifiers(), ke->text());
        QApplication::postEvent(&text_edit_, key_event);
    }

    while (QApplication::hasPendingEvents())
    {
        QApplication::processEvents();
    }
    onyx::screen::instance().enableUpdate(true);

    // Update the line edit.
    onyx::screen::instance().updateWidget(this, onyx::screen::ScreenProxy::DW, true);
}

bool PasswordDialog::event(QEvent * event)
{
    bool ret = OnyxDialog::event(event);
    if (event->type() == QEvent::UpdateRequest)
    {
        onyx::screen::instance().sync(&shadows_.hor_shadow());
        onyx::screen::instance().sync(&shadows_.ver_shadow());
        onyx::screen::instance().updateWidget(this, onyx::screen::ScreenProxy::GU);
    }
    return ret;
}

void PasswordDialog::moveEvent(QMoveEvent *e)
{
    OnyxDialog::moveEvent(e);
}

void PasswordDialog::resizeEvent(QResizeEvent *e)
{
    OnyxDialog::resizeEvent(e);
}

void PasswordDialog::onTimeout()
{
    onyx::screen::instance().updateScreen(onyx::screen::ScreenProxy::GU);
}

}   // namespace ui
