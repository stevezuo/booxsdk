#include "onyx/ui/sign_in_dialog.h"
#include "onyx/screen/screen_proxy.h"
#include "onyx/ui/keyboard_navigator.h"

namespace ui
{

SignInDialog::SignInDialog(QWidget *parent, const QString & title)
    : OnyxDialog(parent)
    , input_layout_(&content_widget_)
    , id_edit_("", 0)
    , password_edit_("", 0)
    , ok_button_(tr("OK"), 0)
    , clear_button_(tr("Clear"), 0)
    , show_plain_text_(tr("Show Plain Text"), 0)
    , keyboard_receiver_(0)
    , keyboard_(0)
{
    createLayout();

    // Widget attributes.
    setModal(true);
    setFocusPolicy(Qt::NoFocus);

    updateTitle(title);
}

SignInDialog::~SignInDialog(void)
{
}

void SignInDialog::createLayout()
{
    const int MARGIN = 10;
    QRect desk = qApp->desktop()->screenGeometry();
    setFixedWidth(desk.width() - 2 * MARGIN);
    title_icon_label_.setPixmap(QPixmap(":/images/dictionary_search.png"));
    content_widget_.setFixedHeight(2 * WIDGET_HEIGHT + 4 * SPACING);

    input_layout_.setContentsMargins(SPACING, 0, SPACING, 0);
    id_label_.setText(tr("User ID "));
    input_layout_.addWidget(&id_label_, 0, 0);
    input_layout_.addWidget(&id_edit_, 0, 1);

    password_label_.setText(tr("Password "));
    input_layout_.addWidget(&password_label_, 1, 0);
    input_layout_.addWidget(&password_edit_, 1, 1);

    id_edit_.setFixedHeight(WIDGET_HEIGHT);
    password_edit_.setFixedHeight(WIDGET_HEIGHT);

    vbox_.addLayout(&hbox_);
    hbox_.addWidget(&show_plain_text_, 800);
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

    // keyboard.
    keyboard_.attachReceiver(this);
    vbox_.addWidget(&keyboard_);

    // Setup connections.
    connect(&show_plain_text_, SIGNAL(clicked()), this, SLOT(onShowPlainTextClicked()), Qt::QueuedConnection);
    connect(&ok_button_, SIGNAL(clicked()), this, SLOT(onOKClicked()));
    connect(&clear_button_, SIGNAL(clicked()), &id_edit_, SLOT(clear()));
    connect(&clear_button_, SIGNAL(clicked()), &password_edit_, SLOT(clear()));
    connect(&timer_, SIGNAL(timeout()), this, SLOT(onTimeout()));
    connect(&id_edit_, SIGNAL(textChanged(const QString&)), this, SLOT(onIDChanged(const QString&)));
    connect(&password_edit_, SIGNAL(textChanged(const QString&)), this, SLOT(onPasswordChanged(const QString&)));
    connect(&id_edit_, SIGNAL(getFocus(OnyxLineEdit *)), this, SLOT(onGetFocus(OnyxLineEdit *)));
    connect(&password_edit_, SIGNAL(getFocus(OnyxLineEdit *)), this, SLOT(onGetFocus(OnyxLineEdit *)));

    // Install event filter
    show_plain_text_.installEventFilter(this);
    ok_button_.installEventFilter(this);
    clear_button_.installEventFilter(this);
    password_edit_.installEventFilter(this);
    id_edit_.installEventFilter(this);
    keyboard_.installEventFilter(this);

    onShowPlainTextClicked();
}


void SignInDialog::onShowPlainTextClicked()
{
    if (!show_plain_text_.isChecked())
    {
        password_edit_.setEchoMode(QLineEdit::Password);
    }
    else
    {
        password_edit_.setEchoMode(QLineEdit::Normal);
    }
}

void SignInDialog::onOKClicked()
{
    shadows_.show(false);
    done(QDialog::Accepted);
}

void SignInDialog::onCloseClicked()
{
    shadows_.show(false);
    done(QDialog::Rejected);
}

void SignInDialog::onIDChanged(const QString & text)
{
}

void SignInDialog::onPasswordChanged(const QString & text)
{
}

/// This function is called by parent widget to display the search widget.
int SignInDialog::popup(const QString & id, const QString & password)
{
    shadows_.show(true);
    if (isHidden())
    {
        show();
    }

    id_edit_.setText(id);
    password_edit_.setText(password);

    if (parentWidget() != 0)
    {
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
    }
    return exec();
}

QString SignInDialog::password()
{
    return password_edit_.text();
}

QString SignInDialog::id()
{
    return id_edit_.text();
}

void SignInDialog::mouseMoveEvent(QMouseEvent *me)
{
    me->accept();
}

void SignInDialog::mousePressEvent(QMouseEvent *me)
{
    me->accept();
}

void SignInDialog::mouseReleaseEvent(QMouseEvent *me)
{
    me->accept();
}

void SignInDialog::keyReleaseEvent(QKeyEvent *ke)
{
    int key = ke->key();
    if (key == Qt::Key_Escape || key == ui::Device_Menu_Key)
    {
        onCloseClicked();
        return;
    }
}

/// The keyPressEvent could be sent from virtual keyboard.
void SignInDialog::keyPressEvent(QKeyEvent * ke)
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

    if ((receiver() != 0 && receiver()->hasFocus()) ||
        (ke->key() != Qt::Key_Down &&
         ke->key() != Qt::Key_Up &&
         ke->key() != Qt::Key_Left &&
         ke->key() != Qt::Key_Right))
    {
        QKeyEvent * key_event = new QKeyEvent(ke->type(), ke->key(), ke->modifiers(), ke->text());
        QApplication::postEvent(receiver(), key_event);
    }

    while (QApplication::hasPendingEvents())
    {
        QApplication::processEvents();
    }
    onyx::screen::instance().enableUpdate(true);

    // Update the line edit.
    onyx::screen::instance().updateWidget(this, onyx::screen::ScreenProxy::DW, true);
}

bool SignInDialog::event(QEvent * event)
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

void SignInDialog::moveEvent(QMoveEvent *e)
{
    OnyxDialog::moveEvent(e);
}

void SignInDialog::resizeEvent(QResizeEvent *e)
{
    OnyxDialog::resizeEvent(e);
}

bool SignInDialog::eventFilter(QObject *obj, QEvent *event)
{
    QWidget *wnd = 0;
    if (event->type() == QEvent::MouseButtonRelease)
    {
        if (obj == &id_edit_ || obj == &password_edit_)
        {
           setReceiver(down_cast<QWidget*>(obj));
        }
    }
    if (event->type() == QEvent::KeyRelease)
    {
        QKeyEvent *key_event = static_cast<QKeyEvent *>(event);
        if (obj == &id_edit_)
        {
            if (key_event->key() == Qt::Key_Down)
            {
                wnd = moveFocus(this, key_event->key());
                if (wnd)
                {
                    wnd->setFocus();
                }
                return true;
            }
            else if (key_event->key() == Qt::Key_Return)
            {
                setReceiver(&id_edit_);
                return true;
            }
        }
        else if (obj == &password_edit_)
        {
            if (key_event->key() == Qt::Key_Down)
            {
                show_plain_text_.setFocus();
                return true;
            }
            else if(key_event->key() == Qt::Key_Up)
            {
                wnd = moveFocus(this, key_event->key());
                if (wnd)
                {
                    wnd->setFocus();
                }
                return true;
            }
            else if (key_event->key() == Qt::Key_Return)
            {
                setReceiver(&password_edit_);
                return true;
            }
        }
        else if (obj == &ok_button_)
        {
            if (key_event->key() == Qt::Key_Left)
            {
                show_plain_text_.setFocus();
                return true;
            }
            else if (key_event->key() == Qt::Key_Right)
            {
                clear_button_.setFocus();
                return true;
            }
            else if (key_event->key() == Qt::Key_Down ||
                     key_event->key() == Qt::Key_Up)
            {
                wnd = moveFocus(this, key_event->key());
                if (wnd)
                {
                    wnd->setFocus();
                }
                return true;
            }
        }
        if (obj == &clear_button_)
        {
            if (key_event->key() == Qt::Key_Left)
            {
                ok_button_.setFocus();
                return true;
            }
            else if (key_event->key() == Qt::Key_Down ||
                     key_event->key() == Qt::Key_Up)
            {
                wnd = moveFocus(this, key_event->key());
                if (wnd)
                {
                    wnd->setFocus();
                }
                return true;
            }
        }
        else if (obj == &show_plain_text_)
        {
            if (key_event->key() == Qt::Key_Right)
            {
                ok_button_.setFocus();
                return true;
            }
            else if (key_event->key() == Qt::Key_Down ||
                     key_event->key() == Qt::Key_Up)
            {
                wnd = moveFocus(this, key_event->key());
                if (wnd)
                {
                    wnd->setFocus();
                }
                return true;
            }
        }
        else if (obj == &keyboard_)
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

void SignInDialog::onTimeout()
{
    onyx::screen::instance().updateScreen(onyx::screen::ScreenProxy::GU);
}

void SignInDialog::onGetFocus(OnyxLineEdit *object)
{
    //setReceiver(object);
}


}   // namespace ui

