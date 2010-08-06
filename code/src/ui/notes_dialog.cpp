#include "onyx/ui/notes_dialog.h"
#include "onyx/screen/screen_proxy.h"

namespace ui
{

NotesDialog::NotesDialog(const QString & text, QWidget *parent)
    : OnyxDialog(parent)
    , hbox_(&content_widget_)
    , vbox1_(0)
    , ok_button_(tr("OK"), this)
    , clear_button_(tr("Clear"), this)
    , content_(text, this)
    , keyboard_(0)
{
    createLayout();

    setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    setFocusPolicy(Qt::NoFocus);
    setModal(true);

    // Install event filter.
    content_.installEventFilter(this);
    keyboard_.installEventFilter(this);
    ok_button_.installEventFilter(this);
    clear_button_.installEventFilter(this);
}

NotesDialog::~NotesDialog()
{
}

void NotesDialog::createLayout()
{
    title_icon_label_.setPixmap(QPixmap(":/images/dictionary_search.png"));
    OnyxDialog::updateTitle(tr("Notes"));
    content_widget_.setFixedHeight(130 + 4 * SPACING);

    // top_vbox1_
    hbox_.setContentsMargins(SPACING, SPACING, SPACING, SPACING);
    hbox_.setSpacing(SPACING);
    hbox_.addWidget(&content_);

    vbox1_.setSpacing(SPACING);
    vbox1_.setSizeConstraint(QLayout::SetMinAndMaxSize);
    vbox1_.setContentsMargins(0, 0, 0, 0);

    hbox_.addLayout(&vbox1_);
    vbox1_.addWidget(&ok_button_);
    vbox1_.addWidget(&clear_button_);

    ok_button_.setFocusPolicy(Qt::NoFocus);
    ok_button_.setFixedHeight(64);
    clear_button_.setFocusPolicy(Qt::NoFocus);
    clear_button_.setFixedHeight(64);
    content_.setFixedHeight(130);

    // Setup connection.
    connect(&ok_button_, SIGNAL(clicked()), this, SLOT(onOKClicked()), Qt::QueuedConnection);
    connect(&clear_button_, SIGNAL(clicked()), &content_, SLOT(clear()), Qt::QueuedConnection);

    // Change buttons attributes.
    content_.setFocusPolicy(Qt::StrongFocus);

    // keyboard.
    keyboard_.attachReceiver(this);
    vbox_.addWidget(&keyboard_);
}

int NotesDialog::popup(const QString & text)
{
    onyx::screen::instance().ensureUpdateFinished();
    onyx::screen::instance().enableUpdate(false);
    shadows_.show(true);
    content_.setText(text);
    if (isHidden())
    {
        show();
    }

    QRect parent_rect = parentWidget()->rect();
    int border = Shadows::PIXELS;
    int width = parent_rect.width() - border * 2;
    if (size().width() != width)
    {
        resize(width, height());
    }

    // adjust position
    int x = Shadows::PIXELS;
    if (!keyboard_.isVisible())
    {
        x = parentWidget()->width() - width - Shadows::PIXELS;
    }
    int y = parentWidget()->height() - height() - Shadows::PIXELS - 30;
    move(x, y);
    onyx::screen::instance().enableUpdate(true);
    onyx::screen::instance().flush(parentWidget(), onyx::screen::ScreenProxy::GU);
    return exec();
}

QSize NotesDialog::sizeHint () const
{
    int height = childrenRect().height();
    if (keyboard_.isHidden())
    {
        return QSize(300, height + 4 * SPACING);
    }
    else
    {
        return QSize(parentWidget()->width(), parentWidget()->height());
    }
}

QSize NotesDialog::minimumSize () const
{
    return sizeHint();
}

QSize NotesDialog::maximumSize () const
{
    return sizeHint();
}

void NotesDialog::mouseMoveEvent(QMouseEvent *me)
{
    me->accept();
}

void NotesDialog::mousePressEvent(QMouseEvent *me)
{
    me->accept();
}

void NotesDialog::mouseReleaseEvent(QMouseEvent *me)
{
    me->accept();
    if (!rect().contains(me->pos()))
    {
        reject();
    }
}

void NotesDialog::keyReleaseEvent(QKeyEvent *ke)
{
    int key = ke->key();
    if (key == Qt::Key_Escape || key == ui::Device_Menu_Key)
    {
        ke->accept();
        onCloseClicked();
        return;
    }

    ke->accept();
}

/// The keyPressEvent could be sent from virtual keyboard.
void NotesDialog::keyPressEvent(QKeyEvent * ke)
{
    ke->accept();
    if (ke->key() == Qt::Key_Shift || ke->key() == Qt::Key_CapsLock)
    {
        return;
    }

    // Disable the parent widget to update screen.
    onyx::screen::instance().enableUpdate(false);

    if (content_.hasFocus() ||
        (ke->key() != Qt::Key_Down &&
         ke->key() != Qt::Key_Up &&
         ke->key() != Qt::Key_Left &&
         ke->key() != Qt::Key_Right))
    {
        QKeyEvent * key_event = new QKeyEvent(ke->type(), ke->key(), ke->modifiers(), ke->text());
        QApplication::postEvent(&content_, key_event);
    }

    while (QApplication::hasPendingEvents())
    {
        QApplication::processEvents();
    }
    onyx::screen::instance().enableUpdate(true);

    // Update the line edit.
    onyx::screen::instance().updateWidget(this, onyx::screen::ScreenProxy::DW, true);
}

bool NotesDialog::eventFilter(QObject *obj, QEvent *event)
{
    QWidget *wnd = 0;
    if (event->type() == QEvent::KeyRelease)
    {
        QKeyEvent *key_event = static_cast<QKeyEvent *>(event);
        if (obj == &ok_button_)
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
            else if (key_event->key() == Qt::Key_Left)
            {
                content_.setFocus();
                return true;
            }
        }
        else if (obj == &clear_button_)
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
            else if (key_event->key() == Qt::Key_Up && ok_button_.isEnabled())
            {
                ok_button_.setFocus();
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

        if (key_event->key() == Qt::Key_Escape)
        {
            onCloseClicked();
        }
    }
    // standard event processing
    return QObject::eventFilter(obj, event);
}

bool NotesDialog::event(QEvent *e)
{
    bool ret = OnyxDialog::event(e);
    if (e->type() == QEvent::UpdateRequest && onyx::screen::instance().isUpdateEnabled())
    {
        onyx::screen::instance().sync(&shadows_.hor_shadow());
        onyx::screen::instance().sync(&shadows_.ver_shadow());
        onyx::screen::instance().updateWidget(parentWidget(), onyx::screen::ScreenProxy::GU);
        e->accept();
        return true;
    }
    return ret;
}

void NotesDialog::moveEvent(QMoveEvent *e)
{
    OnyxDialog::moveEvent(e);
}

void NotesDialog::resizeEvent(QResizeEvent *e)
{
    OnyxDialog::resizeEvent(e);
    adjustPosition();
}

void NotesDialog::hideEvent(QHideEvent * e)
{
    QDialog::hideEvent(e);
    shadows_.show(false);
}

void NotesDialog::onCloseClicked()
{
    shadows_.show(false);
    done(QDialog::Rejected);
}

void NotesDialog::onOKClicked()
{
    shadows_.show(false);
    done(QDialog::Accepted);
}

bool NotesDialog::adjustPosition()
{
    QRect parent_rect = parentWidget()->rect();
    int border = (frameGeometry().width() - geometry().width());
    if (border == 0)
    {
        border = Shadows::PIXELS;
    }

    // Check position.
    QPoint new_pos(border, border);
    if (rect().bottom() < parent_rect.height() / 2)
    {
        new_pos.ry() = parent_rect.height() - height() - border * 2;
    }

    if (pos() != new_pos)
    {
        move(new_pos);
        return true;
    }
    return false;
}

QString NotesDialog::inputText() const
{
    return content_.toPlainText();
}

}   // namespace ui

