#include "onyx/screen/screen_proxy.h"
#include "onyx/base/down_cast.h"
#include "onyx/ui/keyboard_navigator.h"
#include "onyx/ui/search_widget.h"
#include "onyx/ui/keyboard_key.h"

BaseSearchContext::BaseSearchContext(void)
    : pattern_()
    , forward_(true)
    , case_sensitive_(false)
    , match_whole_word_(false)
    , stop_(false)
    , user_data_(0)
{
}

BaseSearchContext::~BaseSearchContext(void)
{
}

void BaseSearchContext::reset()
{
    pattern_.clear();
    forward_ = true;
    case_sensitive_ = false;
    match_whole_word_ = false;
    stop_ = false;
}

void BaseSearchContext::setPattern(const QString &pattern)
{
    pattern_ = pattern;
}

void BaseSearchContext::setForward(bool forward)
{
    forward_ = forward;
}

void BaseSearchContext::setCaseSensitive(bool sensitive)
{
    case_sensitive_ = sensitive;
}

void BaseSearchContext::setMatchWholeWord(bool whole)
{
    match_whole_word_ = whole;
}

namespace ui
{

SearchWidget::SearchWidget(QWidget *parent, BaseSearchContext & ctx)
    : OnyxDialog(parent)
    , hbox_(&content_widget_)
    , text_edit_("", this)
    , search_button_(tr("Search"), this)
    , clear_button_(tr("Clear"), this)
    , search_next_("", this)
    , search_prev_("", this)
    , highlight_all_(tr("Display One"), this)
    , keyboard_(0)
    , ctx_(ctx)
    , full_mode_(true)
    , is_highlight_all_(true)
{
    createLayout();

    // Widget attributes.
    setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    setModal(false);
    setFocusPolicy(Qt::NoFocus);

    // Install event filter
    text_edit_.installEventFilter(this);
    search_button_.installEventFilter(this);
    clear_button_.installEventFilter(this);
    keyboard_.installEventFilter(this);
}

SearchWidget::~SearchWidget(void)
{
}

void SearchWidget::createLayout()
{
    title_icon_label_.setPixmap(QPixmap(":/images/search.png"));

    content_widget_.setFixedHeight(WIDGET_HEIGHT + 4 * SPACING);

    // hbox to layout line edit and buttons.
    hbox_.setContentsMargins(SPACING, SPACING, SPACING, SPACING);
    hbox_.setSpacing(SPACING * 4);

    // Line edit.
    text_edit_.setFixedHeight(WIDGET_HEIGHT);
    hbox_.addWidget(&text_edit_, 400);

    // Buttons.
    search_button_.setFixedHeight(WIDGET_HEIGHT);
    clear_button_.setFixedHeight(WIDGET_HEIGHT);

    search_next_.setIcon(QIcon(":/images/next.png"));
    search_next_.useDefaultHeight();

    search_prev_.setIcon(QIcon(":/images/prev.png"));
    search_prev_.useDefaultHeight();

    // Don't use highlight all now.
    highlight_all_.useDefaultHeight();
    highlight_all_.hide();

    search_button_.setFocusPolicy(Qt::NoFocus);
    clear_button_.setFocusPolicy(Qt::NoFocus);
    search_next_.setFocusPolicy(Qt::NoFocus);
    search_prev_.setFocusPolicy(Qt::NoFocus);
    highlight_all_.setFocusPolicy(Qt::NoFocus);

    hbox_.addWidget(&search_button_);
    hbox_.addWidget(&clear_button_);
    hbox_.addWidget(&search_prev_);
    hbox_.addWidget(&search_next_);
    hbox_.addWidget(&highlight_all_);

    // keyboard.
    keyboard_.attachReceiver(this);
    vbox_.addWidget(&keyboard_);

    // Setup connections.
    connect(&search_button_, SIGNAL(clicked()), this, SLOT(onSearchClicked()), Qt::QueuedConnection);
    connect(&clear_button_, SIGNAL(clicked()), &text_edit_, SLOT(clear()));
    connect(&search_prev_, SIGNAL(clicked()), this, SLOT(onSearchPrevClicked()), Qt::QueuedConnection);
    connect(&search_next_, SIGNAL(clicked()), this, SLOT(onSearchNextClicked()), Qt::QueuedConnection);
    connect(&highlight_all_, SIGNAL(clicked()), this, SLOT(onHighlightAllClicked()));
    connect(&timer_, SIGNAL(timeout()), this, SLOT(onTimeout()));
    connect(&text_edit_, SIGNAL(textChanged(const QString&)), this, SLOT(onTextChanged(const QString&)));
}

/// Update all widget according to the specified parameter.
void SearchWidget::updateChildrenWidgets(bool searching)
{
    if (searching)
    {
        search_next_.show();
        search_prev_.show();
        // highlight_all_.show();
        clear_button_.hide();
        search_button_.hide();
        keyboard_.hide();
        text_edit_.hide();
    }
    else
    {
        search_next_.hide();
        search_prev_.hide();
        highlight_all_.hide();
        clear_button_.show();
        search_button_.show();
        keyboard_.show();
        text_edit_.show();
    }
    adjustSize();
}

void SearchWidget::onSearchClicked()
{
    full_mode_ = false;
    onyx::screen::instance().enableUpdate(false);
    updateChildrenWidgets(true);
    ctx_.setPattern(text_edit_.text());
    updateTitle();
    adjustPosition();
    QApplication::processEvents();
    onyx::screen::instance().enableUpdate(true);
    onyx::screen::instance().flush(parentWidget(), onyx::screen::ScreenProxy::GU);
    readyToSearch(ctx_.forward());
}

void SearchWidget::onSearchNextClicked()
{
    updateTitle();
    readyToSearch(true);
}

void SearchWidget::onHighlightAllClicked()
{
    if ( is_highlight_all_ )
    {
        highlight_all_.setText(tr("Display All"));
        is_highlight_all_ = false;
    }
    else
    {
        highlight_all_.setText(tr("Display One"));
        is_highlight_all_ = true;
    }
    // emit isHighlightAllChanged();
}

void SearchWidget::onSearchPrevClicked()
{
    updateTitle();
    readyToSearch(false);
}

void SearchWidget::onCloseClicked()
{
    // Make sure caller stop searching.
    ctx_.stop(true);
    shadows_.show(false);
    done(QDialog::Rejected);
    full_mode_ = true;
    emit closeClicked();
}

void SearchWidget::onTextChanged(const QString& text)
{
    search_button_.setEnabled(!text.isEmpty());
    clear_button_.setEnabled(!text.isEmpty());
}

/// This function is called by parent widget to display the search widget.
void SearchWidget::ensureVisible()
{
    shadows_.show(true);
    if (isHidden())
    {
        show();
    }

    updateChildrenWidgets(!full_mode_);

    if (full_mode_)
    {
        QRect parent_rect = parentWidget()->rect();
        int border = Shadows::PIXELS;
        int width = parent_rect.width() - border * 2;
        if (size().width() != width)
        {
            resize(width, height());
        }
    }

    update_parent_ = true;
    adjustPosition();

    if (text_edit_.text().isEmpty())
    {
        text_edit_.setText(ctx_.pattern());
    }
    text_edit_.setFocus();
    onTextChanged(text_edit_.text());
    updateTitle();
}

/// Calculate the sizeHint.
QSize SearchWidget::sizeHint () const
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

QSize SearchWidget::minimumSize () const
{
    return sizeHint();
}

QSize SearchWidget::maximumSize () const
{
    return sizeHint();
}

/// This function is called when caller can not find any matched
/// result.
void SearchWidget::noMoreMatches()
{
    // Heave ghosting when using the fast waveform.
    // Use full instead.
    updateTitle(tr("No more matches"));
    onyx::screen::instance().flush();
    onyx::screen::instance().updateWidget(this, onyx::screen::ScreenProxy::GC, false);
}

void SearchWidget::mouseMoveEvent(QMouseEvent *me)
{
    me->accept();
}

void SearchWidget::mousePressEvent(QMouseEvent *me)
{
    me->accept();
}

void SearchWidget::mouseReleaseEvent(QMouseEvent *me)
{
    me->accept();
}

void SearchWidget::keyReleaseEvent(QKeyEvent *ke)
{
    int key = ke->key();
    if (key == Qt::Key_Escape || key == ui::Device_Menu_Key)
    {
        onCloseClicked();
        return;
    }
    else if (key == Qt::Key_PageUp)
    {
        onSearchPrevClicked();
    }
    else if (key == Qt::Key_PageDown)
    {
        onSearchNextClicked();
    }
}

bool SearchWidget::eventFilter(QObject *obj, QEvent *event)
{
    QWidget *wnd = 0;
    if (event->type() == QEvent::KeyRelease)
    {
        QKeyEvent *key_event = static_cast<QKeyEvent *>(event);
        if (obj == &text_edit_)
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
            else if (key_event->key() == Qt::Key_Right && search_button_.isEnabled())
            {
                search_button_.setFocus();
                return true;
            }
        }
        else if (obj == &search_button_)
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
            else if (key_event->key() == Qt::Key_Right && clear_button_.isEnabled())
            {
                clear_button_.setFocus();
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
                wnd = moveFocus(this, key_event->key());
                if (wnd)
                {
                    wnd->setFocus();
                }
                return true;
            }
            else if (key_event->key() == Qt::Key_Left && search_button_.isEnabled())
            {
                search_button_.setFocus();
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

/// The keyPressEvent could be sent from virtual keyboard.
void SearchWidget::keyPressEvent(QKeyEvent * ke)
{
    ke->accept();
    if (ke->key() == Qt::Key_Enter)
    {
        readyToSearch(ctx_.forward());
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


void SearchWidget::readyToSearch(bool forward)
{
    ctx_.setPattern(text_edit_.text());
    ctx_.setForward(forward);
    emit search(ctx_);
}

void SearchWidget::updateTitle(const QString &message)
{
    if (message.isEmpty())
    {
        if (full_mode_)
        {
            OnyxDialog::updateTitle(tr("Search"));
        }
        else
        {
            OnyxDialog::updateTitle(tr("Search") + " " + ctx_.pattern());
        }
    }
    else
    {
        OnyxDialog::updateTitle(message);
    }
}

void SearchWidget::adjustPosition()
{
    int x = Shadows::PIXELS;
    if (!keyboard_.isVisible())
    {
        x = parentWidget()->width() - width() - Shadows::PIXELS;
    }
    int y = parentWidget()->height() - height() - Shadows::PIXELS;
    move(x, y);
}

bool SearchWidget::event(QEvent * event)
{
    bool ret = OnyxDialog::event(event);
    if (event->type() == QEvent::UpdateRequest && onyx::screen::instance().isUpdateEnabled())
    {
        onyx::screen::instance().sync(&shadows_.hor_shadow());
        onyx::screen::instance().sync(&shadows_.ver_shadow());
        if (update_parent_)
        {
            onyx::screen::instance().updateWidget(parentWidget(), onyx::screen::ScreenProxy::GC);
            update_parent_ = false;
        }
        else
        {
            onyx::screen::instance().updateWidget(this, onyx::screen::ScreenProxy::GU);
        }
        event->accept();
    }
    return ret;
}

void SearchWidget::moveEvent(QMoveEvent *e)
{
    OnyxDialog::moveEvent(e);
    update_parent_ = true;
}

void SearchWidget::resizeEvent(QResizeEvent *e)
{
    OnyxDialog::resizeEvent(e);
    adjustPosition();
    update_parent_ = true;
}

void SearchWidget::onTimeout()
{
    onyx::screen::instance().updateScreen(onyx::screen::ScreenProxy::GU);
}

}   // namespace ui
