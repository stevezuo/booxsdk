#include "onyx/ui/keyboard_direction_dialog.h"
#include "onyx/ui/keyboard_layout.h"
#include "onyx/ui/keyboard.h"

namespace ui
{

static const QString DIRECTION_DIALOG_STYLE = "\
QDialog                                 \
{                                       \
     background: transparent;           \
     border-width: 0px;                 \
     border-color: transparent;         \
}";

static const QString KEYBOARD_DIRECTION_BUTTON_STYLE =   "\
QPushButton                             \
{                                       \
    background: transparent;            \
    font-size: 14px;                    \
    border-width: 1px;                  \
    border-color: transparent;          \
    border-style: solid;                \
    color: black;                       \
    padding: 0px;                       \
}                                       \
QPushButton:pressed                     \
{                                       \
    padding-left: 0px;                  \
    padding-top: 0px;                   \
    border-color: black;                \
    background-color: black;            \
}                                       \
QPushButton:checked                     \
{                                       \
    padding-left: 0px;                  \
    padding-top: 0px;                   \
    border-color: black;                \
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

KeyboardDirectionDialog::KeyboardDirectionDialog(KeyboardDirection direction, QWidget *parent)
    : QDialog(parent, static_cast<Qt::WindowFlags>(Qt::WindowStaysOnTopHint|Qt::FramelessWindowHint))
    , layout_(this)
    , direction_button_(tr(""), 0)
    , direction_(direction)
{
    createLayout();
    setStyleSheet(DIRECTION_DIALOG_STYLE);
    setModal(false);
    setBackgroundRole(QPalette::Light);
    setFocusPolicy(Qt::NoFocus);
}

KeyboardDirectionDialog::~KeyboardDirectionDialog()
{
}

void KeyboardDirectionDialog::ensureVisible(KeyboardLayout *layout, bool show_widget)
{
    if (!show_widget && isVisible())
    {
        hide();
        return;
    }

    if (!isVisible() && show_widget)
    {
        show();
    }

    QPoint pos = getDisplayPosition(layout);
    move(pos);
}

QRect KeyboardDirectionDialog::getKeyboardArea()
{
    KeyBoard * keyboard = down_cast<KeyBoard*>(parentWidget());
    shared_ptr<KeyBoardKey> first_key = keyboard->buttons_.front();
    shared_ptr<KeyBoardKey> last_key = keyboard->buttons_.last();
    QRect last_key_rect(last_key->pos(), last_key->size());
    QRect keyboard_rect(first_key->pos(), last_key_rect.bottomRight());

    QPoint pos = first_key->pos();
    QWidget * parent_widget = first_key->parentWidget();
    while (parent_widget != 0)
    {
        pos += parent_widget->pos();
        parent_widget = parent_widget->parentWidget();
    }
    keyboard_rect.moveTo(pos);
    return keyboard_rect;
}

QPoint KeyboardDirectionDialog::getDisplayPosition(KeyboardLayout *layout)
{
    QRect parent_rect = getKeyboardArea();
    QPoint pos(parent_rect.topLeft());

    QSize standard_size = layout->getStandardKeySize();
    int x = 0;
    int y = 0;
    switch (direction_)
    {
    case KEYBOARD_UP:
        {
            x = parent_rect.left() + ((parent_rect.width() - direction_button_.width()) >> 1);
            y = parent_rect.top() + (standard_size.height() - (direction_button_.height() >> 1));
        }
        break;
    case KEYBOARD_DOWN:
        {
            x = parent_rect.left() + ((parent_rect.width() - direction_button_.width()) >> 1);
            y = parent_rect.bottom() - (standard_size.height() - (direction_button_.height() >> 1));
        }
        break;
    case KEYBOARD_LEFT:
        {
            x = parent_rect.left() + (standard_size.width() * 3 + 8) - (direction_button_.width() >> 1);
            y = parent_rect.top() + ((parent_rect.height() - direction_button_.height()) >> 1);
        }
        break;
    case KEYBOARD_RIGHT:
        {
            x = parent_rect.right() - (standard_size.width() * 3 + 8) + (direction_button_.width() >> 1);
            y = parent_rect.top() + ((parent_rect.height() - direction_button_.height()) >> 1);
        }
        break;
    default:
        break;
    }
    pos = QPoint(x, y);
    return pos;
}

void KeyboardDirectionDialog::onDirectionClicked(bool)
{
    emit directionSelected(direction_);
}

void KeyboardDirectionDialog::createLayout()
{
    // top box
    layout_.setSizeConstraint(QLayout::SetMinAndMaxSize);
    layout_.setContentsMargins(0, 0, 0, 0);
    layout_.setSpacing(0);

    direction_button_.setStyleSheet(KEYBOARD_DIRECTION_BUTTON_STYLE);
    QString image_path(":/images/about.png");
    switch (direction_)
    {
    case KEYBOARD_UP:
        image_path = ":/images/keyboard_up.png";
        break;
    case KEYBOARD_DOWN:
        image_path = ":/images/keyboard_down.png";
        break;
    case KEYBOARD_LEFT:
        image_path = ":/images/keyboard_left.png";
        break;
    case KEYBOARD_RIGHT:
        image_path = ":/images/keyboard_right.png";
        break;
    default:
        break;
    }
    QPixmap direction_button_map(image_path);
    direction_button_.setIcon(QIcon(direction_button_map));
    direction_button_.setIconSize(direction_button_map.size());
    direction_button_.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    direction_button_.setFocusPolicy(Qt::NoFocus);

    setMask(direction_button_map.mask());

    layout_.addWidget(&direction_button_);
    connect(&direction_button_, SIGNAL(clicked(bool)), this, SLOT(onDirectionClicked(bool)), Qt::QueuedConnection);
}

void KeyboardDirectionDialog::keyPressEvent(QKeyEvent *ke)
{
    ke->ignore();
}

void KeyboardDirectionDialog::keyReleaseEvent(QKeyEvent *ke)
{
    ke->ignore();
    switch (ke->key())
    {
    case Qt::Key_Up:
        emit directionSelected(KEYBOARD_UP);
        break;
    case Qt::Key_Down:
        emit directionSelected(KEYBOARD_DOWN);
        break;
    case Qt::Key_Left:
        emit directionSelected(KEYBOARD_LEFT);
        break;
    case Qt::Key_Right:
        emit directionSelected(KEYBOARD_RIGHT);
        break;
    case Qt::Key_Return:
        emit directionSelected(KEYBOARD_CENTER);
        break;
    default:
        break;
    }
}

}
