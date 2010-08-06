#include "onyx/ui/keyboard_navigator.h"
#include "onyx/ui/number_widget.h"
#include "onyx/ui/buttons.h"
#include "onyx/screen/screen_proxy.h"

namespace ui
{

static const QString PUSH_BUTTON_STYLE =   "\
QPushButton                             \
{                                       \
    background: white;                  \
    font-size: 20px, bold;              \
    border-width: 1px;                  \
    border-color: black;                \
    border-style: solid;                \
    border-radius: 3;                   \
    color: black;                       \
    padding: 0px;                       \
    min-width: 130px;                   \
    min-height: 32px;                   \
}                                       \
QPushButton:pressed                     \
{                                       \
    padding-left: 0px;                  \
    padding-top: 0px;                   \
    color: white;                       \
    border-color: white;                \
    background-color: black;            \
}                                       \
QPushButton:checked                     \
{                                       \
    padding-left: 0px;                  \
    padding-top: 0px;                   \
    color: white;                       \
    border-color: white;                \
    background-color: black;            \
}                                       \
QPushButton:focus {                     \
    border-width: 2px;                  \
    border-color: black;                \
    border-style: solid;                \
    border-radius: 3;                   \
}                                       \
QPushButton:disabled                    \
{                                       \
    padding-left: 0px;                  \
    padding-top: 0px;                   \
    border-color: dark;                 \
    color: dark;                        \
    background-color: white;            \
}";

static const int COLUMNS = 3;
static const int ROWS    = 4;
static const int MAX     = INT_MAX / 10;

NumberWidget::NumberWidget(QWidget *parent)
    : QWidget(parent)
    , buttons_()
    , number_layout_()
{
    createLayout();
}

NumberWidget::~NumberWidget(void)
{
    clear();
}

void NumberWidget::createLayout()
{
    number_layout_.reset(new QGridLayout(this));
    number_layout_->setContentsMargins(0, 0, 0, 0);
    number_layout_->setSpacing(8);

    // Create buttons.
    QString text;
    for(int i = 0; i < 9; ++i)
    {
        text.setNum(i + 1);
        AbstractButtonPtr ptr(new OnyxPushButton(text, this));
        ptr->setStyleSheet(PUSH_BUTTON_STYLE);
        ptr->setFocusPolicy(Qt::StrongFocus);
        buttons_.push_back(ptr);
        number_layout_->addWidget(ptr.get(), i / COLUMNS, i % COLUMNS);

        connect(ptr.get(), SIGNAL(clicked(bool)),
                this, SLOT(onNumberButtonClicked(bool)), Qt::QueuedConnection);
    }

    // Backspace button.
    {
        AbstractButtonPtr ptr(new OnyxPushButton("", this));
        ptr->setStyleSheet(PUSH_BUTTON_STYLE);
        QPixmap pixmap(":/images/back_number.png");
        ptr->setIcon(QIcon(pixmap));
        ptr->setIconSize(pixmap.size());
        ptr->setFocusPolicy(Qt::StrongFocus);
        buttons_.push_back(ptr);
        number_layout_->addWidget(ptr.get(), ROWS - 1, 0);

        connect(ptr.get(), SIGNAL(clicked(bool)),
                this, SLOT(onBackspaceClicked(bool)), Qt::QueuedConnection);
    }

    // number zero button
    {
        text.setNum(0);
        AbstractButtonPtr ptr(new OnyxPushButton(text, this));
        ptr->setStyleSheet(PUSH_BUTTON_STYLE);
        ptr->setFocusPolicy(Qt::StrongFocus);
        buttons_.push_back(ptr);
        number_layout_->addWidget(ptr.get(), ROWS - 1, 1);
        connect(ptr.get(), SIGNAL(clicked(bool)),
            this, SLOT(onNumberButtonClicked(bool)), Qt::QueuedConnection);
    }

    // OK
    {
        AbstractButtonPtr ptr(new OnyxPushButton("OK", this));
        ptr->setStyleSheet(PUSH_BUTTON_STYLE);
        ptr->setFocusPolicy(Qt::StrongFocus);
        buttons_.push_back(ptr);
        number_layout_->addWidget(ptr.get(), ROWS - 1, 2);
        connect(ptr.get(), SIGNAL(clicked(bool)),
                this, SLOT(onOKClicked(bool)), Qt::QueuedConnection);
    }
}

void NumberWidget::clear()
{
    buttons_.clear();
    number_layout_.reset(0);
}

void NumberWidget::clickFocusButton()
{
    AbstractButtonsIter begin = buttons_.begin();
    AbstractButtonsIter end   = buttons_.end();
    for(AbstractButtonsIter iter = begin; iter != end; ++iter)
    {
        if ((*iter)->hasFocus())
        {
            (*iter)->click();
            return;
        }
    }
}

bool NumberWidget::focusUpDownChild(bool up)
{
    AbstractButtonsIter begin = buttons_.begin();
    AbstractButtonsIter end   = buttons_.end();
    AbstractButtonsIter iter  = begin;
    for(; iter != end; ++iter)
    {
        if ((*iter)->hasFocus())
        {
            break;
        }
    }

    assert(iter != end);
    int index = iter - begin;

    if (up)
    {
        index -= COLUMNS;
        //if (index < 0) index += 12;
        if (index < 0) return false;
    }
    else
    {
        index += COLUMNS;
        //if (index >= 12) index -= 12;
        if (index >= 12) return false;
    }
    buttons_[index]->setFocus();
    return true;
}

void NumberWidget::onNumberButtonClicked(bool checked)
{
    // Downcast. Any other good idea?
    QAbstractButton * btn = static_cast<QAbstractButton *>(sender());
    bool ok = false;
    int number = btn->text().toInt(&ok);

    // Handle overflow.
    if (ok)
    {
        emit numberClicked( number );
    }
}

void NumberWidget::onBackspaceClicked(bool checked)
{
    emit backspaceClicked();
}

void NumberWidget::onOKClicked(bool checked)
{
    emit okClicked();
}

void NumberWidget::keyReleaseEvent(QKeyEvent *e)
{
    e->ignore();
}

}   // namespace ui
