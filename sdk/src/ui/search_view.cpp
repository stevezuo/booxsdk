#include "onyx/screen/screen_proxy.h"

#include "onyx/ui/search_view.h"

namespace vbf
{

const QString PUSH_BUTTON_STYLE = "      \
QPushButton                             \
{                                       \
    font-size: 20px;                    \
    border-width: 1px;                  \
    border-color: black;                \
    border-style: solid;                \
    border-radius: 3;                   \
    color: black;                       \
    padding: 0px;                       \
    background-color: #ffffff;          \
    width : 100;                        \
}                                       \
QPushButton:pressed                     \
{                                       \
    padding-left: 0px;                  \
    padding-top: 0px;                   \
    color: white;                       \
    background-color: #000000;          \
}                                       \
QPushButton:checked                     \
{                                       \
    padding-left: 0px;                  \
    padding-top: 0px;                   \
    color: white;                       \
    background-color: #000000;          \
}";

SearchView::SearchView(QWidget *parent)
    : QWidget(parent)
    , vbox_(this)
    , hbox1_(0)
    //, hbox2_(0)
    , hbox3_(0)
    , keyboard_(this)
    , text_edit_("", this)
    , search_next_("", this)
    , search_prev_("", this)
    , show_tools_("", this)
    , close_("", this)
    //, match_case_(QApplication::tr("Match Case"), this)
    //, match_whole_word_(QApplication::tr("Match Whole Word"), this)
{
    search_next_.setStyleSheet(PUSH_BUTTON_STYLE);
    search_next_.setIcon(QIcon(":/vbf_icons/next.png"));

    search_prev_.setStyleSheet(PUSH_BUTTON_STYLE);
    search_prev_.setIcon(QIcon(":/vbf_icons/prev.png"));

    show_tools_.setStyleSheet(PUSH_BUTTON_STYLE);
    show_tools_.setIcon(QIcon(":/vbf_icons/keyboard.png"));

    close_.setStyleSheet(PUSH_BUTTON_STYLE);
    close_.setIcon(QIcon(":/vbf_icons/close.png"));

    //match_case_.setStyleSheet(PUSH_BUTTON_STYLE);
    //match_whole_word_.setStyleSheet(PUSH_BUTTON_STYLE);

    //match_case_.setCheckable(true);
    //match_whole_word_.setCheckable(true);

    setAutoFillBackground(true);
    createLayout();
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Maximum);

    // setup connection.
    connect(&timer_, SIGNAL(timeout()), this, SLOT(onTimeout()));

    // Disable cursor flash.
    QApplication::setCursorFlashTime(0);

    // setWindowModality(Qt::ApplicationModal);
}

SearchView::~SearchView(void)
{
}

void SearchView::createLayout()
{
    vbox_.setSpacing(2);
    vbox_.setContentsMargins(2, 2, 2, 4);

    // create the first row
    hbox1_.setContentsMargins(0, 0, 0, 20);
    vbox_.addLayout(&hbox1_);
    text_edit_.setFixedHeight(36);
    hbox1_.addWidget(&text_edit_, 400);

    search_prev_.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    connect(&search_prev_,
            SIGNAL(clicked()),
            this,
            SLOT(onSearchPrevClicked()));
    hbox1_.addWidget(&search_prev_, 50);

    search_next_.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    connect(&search_next_,
            SIGNAL(clicked()),
            this,
            SLOT(onSearchNextClicked()));
    hbox1_.addWidget(&search_next_, 50);

    show_tools_.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    connect(&show_tools_,
            SIGNAL(clicked()),
            this,
            SLOT(onShowToolsClicked()));
    hbox1_.addWidget(&show_tools_, 50);

    close_.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    connect(&close_,
            SIGNAL(clicked()),
            this,
            SLOT(onCloseClicked()));
    hbox1_.addWidget(&close_, 50);

    // create the second row
    //vbox_.addLayout(&hbox2_);

    //hbox2_.addWidget(&match_case_);
    //hbox2_.addWidget(&match_whole_word_);

    // create the third row
    //hbox3_.setContentsMargins(0, 20, 0, 0);
    vbox_.addLayout(&hbox3_);
    keyboard_.attachReceiver(&text_edit_);
    hbox3_.addWidget(&keyboard_);
}

void SearchView::onSearchNextClicked()
{
    sendRequest(true);
}

void SearchView::onSearchPrevClicked()
{
    sendRequest(false);
}

QSize SearchView::sizeHint () const
{
    int height = 0;
    int margin_left, margin_top, margin_right, margin_bottom;
    vbox_.getContentsMargins(&margin_left,
                             &margin_top,
                             &margin_right,
                             &margin_bottom);
    height += (margin_top + margin_bottom);
    int width = parentWidget()->width();

    // hbox1_
    hbox1_.getContentsMargins(&margin_left,
                              &margin_top,
                              &margin_right,
                              &margin_bottom);
    height += (margin_top + margin_bottom);
    height += (text_edit_.sizeHint().height() + vbox_.spacing());


    // hbox3_
    if (!keyboard_.isHidden())
    {
        hbox3_.getContentsMargins(&margin_left,
                              &margin_top,
                              &margin_right,
                              &margin_bottom);
        height += (margin_top + margin_bottom);
        height += keyboard_.sizeHint().height() + vbox_.spacing();
    }

    //if (!match_case_.isHidden())
    //{
    //    height += match_case_.sizeHint().height() + vbox_.spacing();
    //}

    qDebug("size width %d", width);
    return QSize(width, height);
}

void SearchView::onCloseClicked()
{
    emit closeClicked();
}

void SearchView::onShowToolsClicked()
{
    displayNormal();
}

void SearchView::displayNormal()
{
    //if (match_case_.isHidden())
    //{
    //    match_case_.show();
    //}
    //if (match_whole_word_.isHidden())
    //{
    //    match_whole_word_.show();
    //}
    if (keyboard_.isHidden())
    {
        keyboard_.show();
    }
    if (!show_tools_.isHidden())
    {
        show_tools_.hide();
    }

    adjustSize();
    adjustPosition();

    if (isHidden())
    {
        show();
    }

    // QApplication::processEvents();
}

void SearchView::mouseMoveEvent(QMouseEvent *me)
{
    me->accept();
}

void SearchView::mousePressEvent(QMouseEvent *me)
{
    me->accept();
}

void SearchView::mouseReleaseEvent(QMouseEvent *me)
{
    me->accept();
}

void SearchView::keyReleaseEvent(QKeyEvent *ke)
{
    ke->ignore();
}

/// The keyPressEvent could be sent from virtual keyboard.
void SearchView::keyPressEvent(QKeyEvent * ke)
{
    ke->accept();
    if (ke->key() == Qt::Key_Enter)
    {
        // By default, search forward.
        sendRequest(true);
        return;
    }
    else if (ke->key() == Qt::Key_Shift || ke->key() == Qt::Key_CapsLock)
    {
        return;
    }

    // Disable the parent widget to update screen.
    QKeyEvent * key_event = new QKeyEvent(ke->type(), ke->key(), ke->modifiers(), ke->text());
    onyx::screen::instance().enableUpdate(false);
    QApplication::postEvent(&text_edit_, key_event);
    QApplication::processEvents();
    onyx::screen::instance().enableUpdate(true);

    // Update the line edit.
    onyx::screen::instance().updateWidget(&text_edit_, onyx::screen::ScreenProxy::DW, false);

    // Launch timer to make sure screen will be updated
    timer_.stop();
    timer_.start(1000);
}


void SearchView::sendRequest(bool forward)
{
    SearchContext ctx;
    ctx.pattern = text_edit_.text();
    ctx.forward = forward;
    //ctx.match_case = match_case_.isChecked();
    //ctx.match_whole_word = match_whole_word_.isChecked();

    //if (!match_case_.isHidden())
    //{
    //    match_case_.hide();
    //}
    //if (!match_whole_word_.isHidden())
    //{
    //    match_whole_word_.hide();
    //}
    if (!keyboard_.isHidden())
    {
        keyboard_.hide();
    }
    if (show_tools_.isHidden())
    {
        show_tools_.show();
    }

    adjustSize();
    adjustPosition();

    emit searchNext(ctx);
}

void SearchView::adjustPosition()
{
    if (parentWidget() == 0)
    {
        return;
    }

    int parent_height = parentWidget()->height();
    move(0, parent_height - height());
}

bool SearchView::event(QEvent * event)
{
    bool ret = QWidget::event(event);
    if (event->type() == QEvent::UpdateRequest)
    {
        static int count = 0;
        qDebug("SearchView update %d", ++count);
        onyx::screen::instance().updateWidget(this);
        event->accept();
        return true;
    }
    return ret;
}

void SearchView::onTimeout()
{
    onyx::screen::instance().updateScreen(onyx::screen::ScreenProxy::GU);
}

}
