#include "onyx/ui/im_char_selection.h"
#include "onyx/ui/keyboard_utils.h"
#include "onyx/screen/screen_proxy.h"

namespace ui
{

static const int MAX_CHARS = 9;
static const int MARGIN = 8;

static const QString PUSH_BUTTON_STYLE =   "\
QPushButton                             \
{                                       \
    background: white;                  \
    font-size: 20px;                    \
    border-width: 1px;                  \
    border-color: black;                \
    border-style: solid;                \
    border-radius: 2;                   \
    color: black;                       \
    padding: 0px;                       \
    min-width: 70px;                    \
    min-height: 40px;                   \
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
    border-radius: 2;                   \
}                                       \
QPushButton:disabled                    \
{                                       \
    padding-left: 0px;                  \
    padding-top: 0px;                   \
    border-color: dark;                 \
    color: dark;                        \
    background-color: white;            \
}";

static int getKeyWidth(shared_ptr<OnyxPushButton> key)
{
    QFontMetrics fm(key->font());
    QString text = key->text();
    int w = fm.width(text) + MARGIN * 2;
    if (w < 52)
    {
        w = 52;
    }
    return w;
}

InputMethodCharSelection::InputMethodCharSelection(QWidget* parent)
    : QWidget(parent, Qt::FramelessWindowHint)
    , ver_layout_(0)
    , button_group_(0)
{
    init();
}

InputMethodCharSelection::~InputMethodCharSelection()
{
}

void InputMethodCharSelection::displayCharacters(const QStringList & texts)
{
    candidates_ = texts;
    for (int i = 0; i < MAX_CHARS; ++i)
    {
        shared_ptr<OnyxPushButton> key = buttons_.at(i);
        if (i < candidates_.size())
        {
            QString text = candidates_.at(i);
            if (text.isEmpty())
            {
                continue;
            }
            key->setVisible(true);
            key->setText(text);
            key->setEnabled(true);
        }
        else
        {
            key->setVisible(false);
        }
    }

    int width_sum = 0;
    for (int i = 0; i < MAX_CHARS; ++i)
    {
        shared_ptr<OnyxPushButton> key = buttons_.at(i);
        int width = getKeyWidth(key);
        key->setFixedWidth(width);

        int sum = width_sum + width;
        if (width_sum > 480)
        {
            key->setVisible(false);
        }
        else
        {
            width_sum = sum;
        }
    }
}

void InputMethodCharSelection::hideCharacters()
{
    for (int i = 0; i < MAX_CHARS; ++i)
    {
        shared_ptr<OnyxPushButton> key = buttons_.at(i);
        key->setEnabled(false);
    }
}

void InputMethodCharSelection::init()
{
    // reset the layout
    buttons_.clear();
    hor_layouts_.clear();

    ver_layout_ = new QVBoxLayout(this);
    ver_layout_->setAlignment(Qt::AlignCenter);
    ver_layout_->setSpacing(4);
    ver_layout_->setContentsMargins(0, 0, 0, 0);

    button_group_.reset(new QButtonGroup(this));

    setAutoFillBackground(true);
    setBackgroundRole(QPalette::Button);
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Maximum);

    button_group_->setExclusive(false);

    shared_ptr<QHBoxLayout> hor_layout1(new QHBoxLayout());
    hor_layouts_.push_back(hor_layout1);
    hor_layout1->setAlignment(Qt::AlignCenter);
    hor_layout1->setSpacing(4);
    hor_layout1->setContentsMargins(0, 0, 0, 0);

    shared_ptr<KeyBoardKey> blank_key(new KeyBoardKey(this));
    shared_ptr<KeyBoardKey> back_key(new KeyBoardKey(this));
    shared_ptr<KeyBoardKey> char_key(new KeyBoardKey(this));
    shared_ptr<KeyBoardKey> enter_key(new KeyBoardKey(this));

    blank_key->setCode(Blank);
    back_key->setCode(BackSpace);
    char_key->setCode(HandWriting);
    enter_key->setCode(EnterCode);

    char_key->setText(QCoreApplication::tr("Keyboard"));
    char_key->setFixedWidth(140);

    blank_key->setFocusPolicy(Qt::NoFocus);
    back_key->setFocusPolicy(Qt::NoFocus);
    char_key->setFocusPolicy(Qt::NoFocus);
    enter_key->setFocusPolicy(Qt::NoFocus);

    hor_layout1->addWidget(blank_key.get());
    hor_layout1->addWidget(back_key.get());
    hor_layout1->addWidget(enter_key.get());
    hor_layout1->addWidget(char_key.get());
    keys_.push_back(blank_key);
    keys_.push_back(back_key);
    keys_.push_back(char_key);
    keys_.push_back(enter_key);

    ver_layout_->addLayout(hor_layout1.get());

    shared_ptr<QHBoxLayout> hor_layout2(new QHBoxLayout());
    hor_layouts_.push_back(hor_layout2);
    hor_layout2->setAlignment(Qt::AlignCenter);
    hor_layout2->setSpacing(4);
    hor_layout2->setContentsMargins(0, 0, 0, 0);

    // initialize the keys
    for (int i = 0; i < MAX_CHARS; ++i)
    {
        shared_ptr<OnyxPushButton> key(new OnyxPushButton("", this));
        key->setStyleSheet(PUSH_BUTTON_STYLE);
        key->setText(QChar(0xFF10 + i));

        hor_layout2->addWidget(key.get());
        button_group_->addButton(key.get());
        buttons_.push_back(key);
    }

    button_group_->addButton(blank_key.get());
    button_group_->addButton(back_key.get());
    button_group_->addButton(char_key.get());
    button_group_->addButton(enter_key.get());

    ver_layout_->addLayout(hor_layout2.get());

    connect(button_group_.get(),
            SIGNAL(buttonClicked(QAbstractButton*)),
            this,
            SLOT(onButtonClicked(QAbstractButton*)),
            Qt::QueuedConnection);
}

void InputMethodCharSelection::onButtonClicked(QAbstractButton *button)
{
    KeyBoardKey *key = dynamic_cast<KeyBoardKey*>(button);
    if (key != 0)
    {
        uint code = static_cast<uint>(key->code());

        switch (code)
        {
        case Blank:
        case BackSpace:
        case HandWriting:
        case EnterCode:
            emit buttonClicked(button);
            return;
        }
    }
    else
    {
        OnyxPushButton *text_key = dynamic_cast<OnyxPushButton*>(button);
        if (text_key != 0)
        {
            int index = 0;
            for (int i = 0; i < buttons_.size(); ++i)
            {
                OnyxPushButton * button = buttons_[i].get();
                if (button == text_key)
                {
                    index = i;
                    break;
                }
            }
            emit textSelected(text_key->text(), index);
        }
    }
}

}
