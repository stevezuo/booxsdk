#include "onyx/sys/sys.h"
#include "onyx/screen/screen_proxy.h"
#include "onyx/data/sketch_proxy.h"
#include "onyx/ui/keyboard.h"
#include "onyx/ui/keyboard_language_dialog.h"
#include "onyx/data/handwriting_manager.h"
#include "onyx/data/handwriting_widget.h"
#include "onyx/data/handwriting_functions_model.h"

using namespace handwriting;

namespace ui
{

namespace
{

QLocale::Country currentCountry()
{
    QLocale locale = QLocale::system();
    return locale.country();
}

QLocale::Language currentLanguage()
{
    QLocale locale = QLocale::system();
    return locale.language();
}

}

static const int finish_charater_interval = 1000;
static const int auto_select_interval = 2500;


//int standardKeySize()
//{
//    if (isRussian() && !isEnglishLayout())
//    {
//        return 43;
//    }
//    else if (isSwedish())
//    {
//        return 47;
//    }
//    else if (isPolish())
//    {
//        return 43;
//    }
//    else if (isHungarian())
//    {
//        return 43;
//    }
//    return 52;
//}

//void initPolishCode(QVector<QChar> & code_vector)
//{
//    initEnglishCode(code_vector);
//    code_vector.push_back(QChar(0x00D3));   //O acute  // 26
//    code_vector.push_back(QChar(0x00F3));   //o acute  // 27
//    code_vector.push_back(QChar(0x0104));  //A ogonek   // 28
//    code_vector.push_back(QChar(0x0105));  //a ogonek   // 29
//    code_vector.push_back(QChar(0x0106));  //C acute   // 30
//    code_vector.push_back(QChar(0x0107));  //c acute  // 31
//    code_vector.push_back(QChar(0x0118));  //E ogonek  // 32
//    code_vector.push_back(QChar(0x0119));  //e ogonek   // 33
//    code_vector.push_back(QChar(0x0143));  //N acute   // 34
//    code_vector.push_back(QChar(0x0144));  //n acute   // 35
//    code_vector.push_back(QChar(0x015A));  //S acute   // 36
//    code_vector.push_back(QChar(0x015B));  //s acute   // 37
//    code_vector.push_back(QChar(0x0179));  //Z acute  // 38
//    code_vector.push_back(QChar(0x017A));  //z acute   // 39
//    code_vector.push_back(QChar(0x017B));  //Z dot above   // 40
//    code_vector.push_back(QChar(0x017C));  //z dot above   // 41
//    code_vector.push_back(QChar(0x0141));  //L stroke  // 42
//    code_vector.push_back(QChar(0x0142));  //l stroke  // 43
//}
//void initHungarianCode(QVector<QChar> & code_vector)
//{
//    initEnglishCode(code_vector);
//    code_vector.push_back(QChar(0x00C1));   //A acute  // 26
//    code_vector.push_back(QChar(0x00E1));   //a acute  // 27
//    code_vector.push_back(QChar(0x00C9));  //E acute   // 28
//    code_vector.push_back(QChar(0x00E9));  //e acute   // 29
//    code_vector.push_back(QChar(0x00CD));  //I acute   // 30
//    code_vector.push_back(QChar(0x00ED));  //i acute  // 31
//    code_vector.push_back(QChar(0x00D3));   //O acute  // 32
//    code_vector.push_back(QChar(0x00F3));   //o acute  // 33
//    code_vector.push_back(QChar(0x00D6));  //O double dot above   // 34
//    code_vector.push_back(QChar(0x00F6));  //o double dot above   // 35
//    code_vector.push_back(QChar(0x0150));  //O double acute   // 36
//    code_vector.push_back(QChar(0x0151));  //o double acute   // 37
//    code_vector.push_back(QChar(0x00DA));  //U acute  // 38
//    code_vector.push_back(QChar(0x00FA));  //u acute   // 39
//    code_vector.push_back(QChar(0x00DC));  //U double dot above   // 40
//    code_vector.push_back(QChar(0x00FC));  //u double dot above   // 41
//    code_vector.push_back(QChar(0x0170));  //U double acute  // 42
//    code_vector.push_back(QChar(0x0171));  //u double acute  // 43
//}

//void initPolishKeyboard()
//{
//    int index = 0;
//    if (!keyboard_standard[0].isEmpty())
//    {
//        keyboard_standard[0].clear();
//    }
//    keyboard_standard[0].resize(12);
//    setKey(0, index++, '1');
//    setKey(0, index++, '2');
//    setKey(0, index++, '3');
//    setKey(0, index++, '4');
//    setKey(0, index++, '5');
//    setKey(0, index++, '6');
//    setKey(0, index++, '7');
//    setKey(0, index++, '8');
//    setKey(0, index++, '9');
//    setKey(0, index++, '0');
//    setKey(0, index++, code_vector[27]);
//    setKey(0, index++, code_vector[29]);
//
//    if (!keyboard_standard[1].isEmpty())
//    {
//        keyboard_standard[1].clear();
//    }
//    keyboard_standard[1].resize(12);
//    index = 0;
//    setKey(1, index++, code_vector[16]);
//    setKey(1, index++, code_vector[22]);
//    setKey(1, index++, code_vector[4]);
//    setKey(1, index++, code_vector[17]);
//    setKey(1, index++, code_vector[19]);
//    setKey(1, index++, code_vector[24]);
//    setKey(1, index++, code_vector[20]);
//    setKey(1, index++, code_vector[8]);
//    setKey(1, index++, code_vector[14]);
//    setKey(1, index++, code_vector[15]);
//    setKey(1, index++, code_vector[31]);
//    setKey(1, index++, code_vector[33]);
//
//    if (!keyboard_standard[2].isEmpty())
//    {
//        keyboard_standard[2].clear();
//    }
//    keyboard_standard[2].resize(12);
//    index = 0;
//    setKey(2, index++, code_vector[0]);
//    setKey(2, index++, code_vector[18]);
//    setKey(2, index++, code_vector[3]);
//    setKey(2, index++, code_vector[5]);
//    setKey(2, index++, code_vector[6]);
//    setKey(2, index++, code_vector[7]);
//    setKey(2, index++, code_vector[9]);
//    setKey(2, index++, code_vector[10]);
//    setKey(2, index++, code_vector[11]);
//    setKey(2, index++, code_vector[12]);
//    setKey(2, index++, code_vector[35]);
//    setKey(2, index++, code_vector[37]);
//
//    if (!keyboard_standard[3].isEmpty())
//    {
//        keyboard_standard[3].clear();
//    }
//    keyboard_standard[3].resize(9);
//    index = 0;
//    setKeyUnicode(3, index++, CapLock);
//    setKey(3, index++, code_vector[25]);
//    setKey(3, index++, code_vector[23]);
//    setKey(3, index++, code_vector[2]);
//    setKey(3, index++, code_vector[21]);
//    setKey(3, index++, code_vector[1]);
//    setKey(3, index++, code_vector[13]);
//    setKeyUnicode(3, index++, BackSpace);
//    setKey(3, index++, code_vector[43]);
//
//    if (!keyboard_standard[4].isEmpty())
//    {
//        keyboard_standard[4].clear();
//    }
//    bool has_touch = SysStatus::instance().hasTouchScreen();
//    keyboard_standard[4].resize(has_touch ? 6 : 5);
//    index = 0;
//    setKeyUnicode(4, index++, ShiftCode);
//    setKeyUnicode(4, index++, Blank);
//    setKeyUnicode(4, index++, EnterCode);
//    if (has_touch)
//    {
//        setKeyUnicode(4, index++, HandWriting);
//    }
//    setKey(4, index++, code_vector[39]);
//    setKey(4, index++, code_vector[41]);
//}
//
//void initHungarianKeyboard()
//{
//    //hungarian keyboard has the same key arrangement as polish keyboard
//    initPolishKeyboard();
//}

bool isUpKey(const Keys & keys, int row, int column)
{
    if ( row == 0 )
    {
        return true;
    }
    return false;
}

bool isDownKey(const Keys & keys, int row, int column)
{
    if (row == (keys.size() - 1))
    {
        return true;
    }
    return false;
}

bool isLeftKey(const Keys & keys, int row, int column)
{
    if (row > 0 && row < (keys.size() - 1))
    {
        if (column < 3)
        {
            return true;
        }
    }
    return false;
}

bool isRightKey(const Keys & keys, int row, int column)
{
    if (row > 0 && row < (keys.size() - 1))
    {
        if (column > (keys[row].size() - 4) && column < keys[row].size())
        {
            return true;
        }
    }
    return false;
}

KeyBoard::KeyBoard(QWidget* parent, Qt::WFlags f)
    : QFrame(parent, f)
    , shift_(false)
    , lock_(false)
    , is_handwriting_(false)
    , keyboard_layout_(0)
    , ver_layout_(0)
    , button_group_(0)
    , receiver_(0)
    , sketch_proxy_(0)
    , finish_character_timer_(finish_charater_interval, this, SLOT(onFinishCharacterTimeOut()))
    , auto_select_timer_(auto_select_interval, this, SLOT(onAutoSelect()))
    , up_arrow_(0)
    , down_arrow_(0)
    , left_arrow_(0)
    , right_arrow_(0)
{
    init(QLocale::system());
}

KeyBoard::~KeyBoard()
{
}

void KeyBoard::resetState()
{
}

void KeyBoard::init(const QLocale & locale)
{
    shift_ = false;
    lock_  = false;
    keyboard_layout_.reset(keyboard_layout_provider_.getLayout(locale));
    Keys & keys = keyboard_layout_->keys();

    // reset the layout
    buttons_.clear();
    hor_layouts_.clear();
    QLayout * prev_layout = layout();
    if (prev_layout != 0)
    {
        delete prev_layout;
    }

    ver_layout_ = new QVBoxLayout(this);
    button_group_.reset(new QButtonGroup(this));

    setAutoFillBackground(true);
    setBackgroundRole(QPalette::Button);

    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Maximum);

    ver_layout_->setAlignment(Qt::AlignCenter);
    ver_layout_->setSpacing(4);
    ver_layout_->setContentsMargins(0, 4, 0, 4);

    button_group_->setExclusive(false);

    // add the horizontal layouts into vertical layout.
    for (int i = 0; i < keys.size(); ++i)
    {
        shared_ptr<QHBoxLayout> hor_layout(new QHBoxLayout());
        hor_layouts_.push_back(hor_layout);
        hor_layout->setAlignment(Qt::AlignCenter);
        hor_layout->setSpacing(4);
        hor_layout->setContentsMargins(0, 0, 0, 0);
        for (int k = 0; k < keys[i].size(); ++k)
        {
            uint code = keys[i][k];
            QSize key_size = keyboard_layout_->getKeySize(code);
            shared_ptr<KeyBoardKey> key(new KeyBoardKey(keyboard_layout_.get(), this));

            int key_direction = 0;
            if (isUpKey(keys, i, k))
            {
                up_buttons_.push_back(key);
                key_direction = KEYBOARD_UP;
            }
            else if (isDownKey(keys, i, k))
            {
                down_buttons_.push_back(key);
                key_direction = KEYBOARD_DOWN;
            }
            else if (isLeftKey(keys, i, k))
            {
                left_buttons_.push_back(key);
                key_direction = KEYBOARD_LEFT;
            }
            else if (isRightKey(keys, i, k))
            {
                right_buttons_.push_back(key);
                key_direction = KEYBOARD_RIGHT;
            }
            else
            {
                center_buttons_.push_back(key);
                key_direction = KEYBOARD_CENTER;
            }
            key->setCode(code, key_direction);

            // connect the signals
            connect(this, SIGNAL(shifted(bool)), key.get(), SLOT(onShifted(bool)));
            connect(this, SIGNAL(capLocked(bool)), key.get(), SLOT(onCapLocked(bool)));

            hor_layout->addWidget(key.get());
            button_group_->addButton(key.get());
            buttons_.push_back(key);


        }
        ver_layout_->addLayout(hor_layout.get());
    }

    im_char_selection_.reset(new InputMethodCharSelection(keyboard_layout_.get(), this));
    ver_layout_->addWidget(im_char_selection_.get());
    im_char_selection_->setVisible(false);
    connect(im_char_selection_.get(), SIGNAL(textSelected(const QString&, int)),
            this, SLOT(onTextSelected(const QString&, int)), Qt::QueuedConnection);
    connect(im_char_selection_.get(), SIGNAL(buttonClicked(QAbstractButton *)),
            this, SLOT(onButtonClicked(QAbstractButton *)), Qt::QueuedConnection);

    shared_ptr<QHBoxLayout> handwriting_layout(new QHBoxLayout());
    hor_layouts_.push_back(handwriting_layout);
    handwriting_layout->setAlignment(Qt::AlignCenter);
    handwriting_layout->setSpacing(4);
    handwriting_layout->setContentsMargins(0, 0, 0, 0);
    ver_layout_->addLayout(handwriting_layout.get());

    handwriting_widget_.reset(new HandwritingWidget(this));
    handwriting_widget_->setFixedSize(QSize(400, 200));
    if (!is_handwriting_)
    {
        handwriting_widget_->setVisible(false);
    }
    handwriting_layout->addWidget(handwriting_widget_.get());

    HandwritingFunctionsModel::instance().getModel(handwriting_functions_model_);
    handwriting_functions_view_.reset(new OnyxTreeView(this, &handwriting_functions_model_));
    handwriting_functions_view_->setFixedSize(QSize(156, 200));
    handwriting_functions_view_->showHeader(false);
    if (!is_handwriting_)
    {
        handwriting_functions_view_->setVisible(false);
    }
    handwriting_layout->addWidget(handwriting_functions_view_.get());

    // set current locale
    QLocale current_locale = QLocale::system();
    QModelIndex index = HandwritingFunctionsModel::instance().getIndexBySetting(current_locale.language(),
                                                                                handwriting_functions_model_);
    if (index.isValid())
    {
        handwriting_functions_view_->select(index);
    }

    connect(handwriting_functions_view_.get(), SIGNAL(activated(const QModelIndex &)),
            this, SLOT(onHandwritingFunctionClicked(const QModelIndex &)));
    connect(button_group_.get(), SIGNAL(buttonClicked(QAbstractButton*)),
            this, SLOT(onButtonClicked(QAbstractButton*)), Qt::QueuedConnection);
}

void KeyBoard::keyReleaseEvent(QKeyEvent *e)
{
    e->ignore();
}

void KeyBoard::resizeEvent(QResizeEvent *re)
{
    QFrame::resizeEvent(re);
}

void KeyBoard::showEvent(QShowEvent *se)
{
    QTimer::singleShot(1000, this, SLOT(onDisplayArrows()));
    QFrame::showEvent(se);
}

void KeyBoard::hideEvent(QHideEvent *he)
{
    displayDirectionArrows(false);
    QFrame::hideEvent(he);
}

void KeyBoard::onDisplayArrows()
{
    displayDirectionArrows(true);
    center_buttons_[(center_buttons_.size() - 1) >> 1]->setFocus();
}

bool KeyBoard::event(QEvent *e)
{
    if (e->type() == QEvent::LocaleChange)
    {
        HandwritingManager::instance().reload();
    }
    return QFrame::event(e);
}

void KeyBoard::handleCapLockPressed()
{
    lock_ = !lock_;
    emit capLocked(lock_);
    onyx::screen::instance().flush(this, onyx::screen::ScreenProxy::GC, false);
}

void KeyBoard::handleShiftPressed()
{
    shift_ = !shift_;
    emit shifted(shift_);
    onyx::screen::instance().flush(this, onyx::screen::ScreenProxy::GC, false);
}

void KeyBoard::handleSwitchLanguagePressed()
{
    KeyboardLanguageDialog dialog(this, keyboard_layout_provider_.currentLocale());
    if (dialog.exec() == QDialog::Rejected)
    {
        return;
    }
    hide();
    init(dialog.currentLocale());
    show();
    onyx::screen::instance().flush(0, onyx::screen::ScreenProxy::GC);
}

void KeyBoard::onHandwritingFunctionClicked(const QModelIndex & index)
{
    int item = index.data(Qt::UserRole + 1).toInt();
    HandwritingFunctionsModel::instance().onItemClicked(index, handwriting_functions_model_);

    // switch locale or other settings
    if (item > 0)
    {
        HandwritingManager::instance().setLocale(QLocale(static_cast<QLocale::Language>(item)));
    }
    else
    {
        HandwritingManager::instance().setSpecialRecognizeRange(static_cast<SpecialRecognizeRange>(item));
    }

    handwriting_functions_view_->setModel(&handwriting_functions_model_);

    QModelIndex model_index = HandwritingFunctionsModel::instance().getIndexBySetting(item,
                                                                                      handwriting_functions_model_);
    if (model_index.isValid())
    {
        handwriting_functions_view_->select(model_index);
    }
}

void KeyBoard::handleHandWriting()
{
    is_handwriting_ = !is_handwriting_;
    if (sketch_proxy_ == 0)
    {
        sketch_proxy_.reset(new sketch::SketchProxy());
        connect(sketch_proxy_.get(), SIGNAL(strokeStarted()), this, SLOT(handleStrokeStarted()));
        connect(sketch_proxy_.get(), SIGNAL(pointAdded(SketchPoint)), this, SLOT(handlePointAdded(SketchPoint)));
        connect(sketch_proxy_.get(), SIGNAL(strokeAdded(const Points &)), this, SLOT(handleStrokeAdded(const Points &)));
    }

    onyx::screen::instance().enableUpdate(false);
    im_char_selection_->hideCharacters();

    for (int i = 0; i < buttons_.size(); ++i)
    {
        //buttons_.at(i)->setFocusPolicy(is_handwriting_ ? Qt::NoFocus : Qt::StrongFocus);
        buttons_.at(i)->setVisible(!is_handwriting_);
    }

    handwriting_widget_->setVisible(is_handwriting_);
    handwriting_functions_view_->setVisible(is_handwriting_);
    im_char_selection_->setVisible(is_handwriting_);
    if (!is_handwriting_)
    {
        handwriting_widget_->deattachSketchProxy();
    }
    else
    {
        handwriting_widget_->setFocus();
        handwriting_widget_->attachSketchProxy(sketch_proxy_.get());
    }

    // update the widget
    onyx::screen::instance().enableUpdate(true);
    onyx::screen::instance().flush(0, onyx::screen::ScreenProxy::GC);
}

void KeyBoard::onStrokeStarted()
{
    auto_select_timer_.stop();
    finish_character_timer_.stop();
}

void KeyBoard::onPointAdded(SketchPoint point)
{
    // collect point
    HandwritingManager::instance().collectPoint(point.x(), point.y());
}

void KeyBoard::onStrokeAdded(const Points & points)
{
    // finish stroke
    HandwritingManager::instance().finishStroke();

    // start to check whether finishing the character
    finish_character_timer_.start();
}

void KeyBoard::onFinishCharacterTimeOut()
{
    HandwritingManager & handwriting_mgr = HandwritingManager::instance();

    // finish character
    handwriting_mgr.finishCharacter();

    // recognize the character
    handwriting_mgr.recognize(candidates_);

    // clear the strokes on keyboard
    handwriting_widget_->onClearSketches();

    // clear all of the points
    handwriting_mgr.clearPoints();

    // clear current text
    current_text_.clear();

    if (candidates_.isEmpty())
    {
        // do nothing
    }
    else
    {
        // pop up im character selection
        im_char_selection_->displayCharacters(candidates_);
    }

    // update the widget
    onyx::screen::instance().flush(this, onyx::screen::ScreenProxy::GC, false);

    // start automatic selection timer
    auto_select_timer_.start();
}

void KeyBoard::onTextSelected(const QString & text, int index)
{
    // stop the auto selection timer if necessary
    auto_select_timer_.stop();

    // hide the im char selection widget
    onyx::screen::instance().enableUpdate(false);
    handwriting_widget_->setFocus();
    im_char_selection_->hideCharacters();
    onyx::screen::instance().enableUpdate(true);
    onyx::screen::instance().flush(this, onyx::screen::ScreenProxy::GU, false);

    // emit the text to receiver
    for (int i = 0; i < text.size(); ++i)
    {
        QChar ch = text.at(i);
        postKeyEvent(QEvent::KeyPress, ch.unicode());
    }

    if (!candidates_.isEmpty())
    {
        candidates_.clear();
    }

    if (!current_text_.isEmpty())
    {
        adjustAssociatedChar(current_text_, index);
    }

    // display associated characters
    displayAssociatedChars(text);
}

void KeyBoard::displayAssociatedChars(const QString & current_text)
{
    HandwritingManager & handwriting_mgr = HandwritingManager::instance();
    current_text_ = current_text;
    if (handwriting_mgr.getAssociatedChar(current_text, candidates_))
    {
        onyx::screen::instance().flush();

        // pop up im character selection
        im_char_selection_->displayCharacters(candidates_);

        // update the widget
        onyx::screen::instance().flush(im_char_selection_.get(), onyx::screen::ScreenProxy::GC, false);
    }
}

bool KeyBoard::adjustAssociatedChar(const QString & dst_text, int index)
{
    HandwritingManager & handwriting_mgr = HandwritingManager::instance();
    return handwriting_mgr.adjustAssociatedChar(dst_text, index);
}

void KeyBoard::onAutoSelect()
{
    if (!candidates_.isEmpty())
    {
        QString best = candidates_.at(0);
        onTextSelected(best, 0);
    }
}

void KeyBoard::updateModifiers()
{
    modifiers_ = Qt::NoModifier;
    if (shift_)
    {
        modifiers_ |= Qt::ShiftModifier;
    }

    // TODO. Add support for ctrl and alt
}

QChar KeyBoard::getRealChar(uint code)
{
    QChar charactor;
    if (code < BSCode || code > UnknownCode)
    {
        charactor = QChar(code);
        if (charactor.isLetter())
        {
            if (lock_)
            {
                charactor = charactor.toUpper();
            }

            if (shift_)
            {
                if (charactor.isUpper())
                {
                    charactor = charactor.toLower();
                }
                else if (charactor.isLower())
                {
                    charactor = charactor.toUpper();
                }
            }
        }
    }

    if (code == Blank)
    {
        charactor = ' ';
    }
    return charactor;
}

void KeyBoard::postKeyEvent(QEvent::Type type, uint code)
{
    if (receiver_ == 0)
    {
        qDebug("No Receiver");
        return;
    }

    int key_code = keyboard_layout_->getQKeyCode(code);
    QString text = getRealChar(code);

    // Post the key event to receiver
    QKeyEvent * key_event = new QKeyEvent(type, key_code, modifiers_, text);
    QApplication::postEvent(receiver_, key_event);
}

void KeyBoard::onButtonClicked(QAbstractButton *button)
{
    KeyBoardKey *key = dynamic_cast<KeyBoardKey*>(button);

    uint code = key->code();
    switch (code)
    {
    case CapLock:
        handleCapLockPressed();
        break;
    case ShiftCode:
        handleShiftPressed();
        break;
    case SwitchLanguage:
        handleSwitchLanguagePressed();
        break;
    case HandWriting:
        handleHandWriting();
        break;
    default:
        break;
    }

    updateModifiers();
    postKeyEvent(QEvent::KeyPress, code);
    displayDirectionArrows(true);
}

void KeyBoard::displayDirectionArrows(bool display)
{
    onyx::screen::instance().enableUpdate(false);
    if (up_arrow_ == 0)
    {
        up_arrow_.reset(new KeyboardDirectionDialog(KEYBOARD_UP, this));
        connect(up_arrow_.get(), SIGNAL(directionSelected(KeyboardDirection)),
                this, SLOT(onDirectionSelected(KeyboardDirection)));
    }
    if (down_arrow_ == 0)
    {
        down_arrow_.reset(new KeyboardDirectionDialog(KEYBOARD_DOWN, this));
        connect(down_arrow_.get(), SIGNAL(directionSelected(KeyboardDirection)),
                this, SLOT(onDirectionSelected(KeyboardDirection)));
    }
    if (left_arrow_ == 0)
    {
        left_arrow_.reset(new KeyboardDirectionDialog(KEYBOARD_LEFT, this));
        connect(left_arrow_.get(), SIGNAL(directionSelected(KeyboardDirection)),
                this, SLOT(onDirectionSelected(KeyboardDirection)));
    }
    if (right_arrow_ == 0)
    {
        right_arrow_.reset(new KeyboardDirectionDialog(KEYBOARD_RIGHT, this));
        connect(right_arrow_.get(), SIGNAL(directionSelected(KeyboardDirection)),
                this, SLOT(onDirectionSelected(KeyboardDirection)));
    }
    up_arrow_->ensureVisible(keyboard_layout_.get(), display);
    down_arrow_->ensureVisible(keyboard_layout_.get(), display);
    left_arrow_->ensureVisible(keyboard_layout_.get(), display);
    right_arrow_->ensureVisible(keyboard_layout_.get(), display);
    onyx::screen::instance().enableUpdate(true);
    onyx::screen::instance().flush(this, onyx::screen::ScreenProxy::GC4, false);
}

void KeyBoard::onDirectionSelected(KeyboardDirection direction)
{
    qDebug("Direction selected!");
    displayDirectionArrows(false);

    int index = 0;
    switch (direction)
    {
    case KEYBOARD_UP:
        {
            index = (up_buttons_.size() - 1) >> 1;
            up_buttons_[index]->setFocus();
        }
        break;
    case KEYBOARD_DOWN:
        {
            index = (down_buttons_.size() - 1) >> 1;
            down_buttons_[index]->setFocus();
        }
        break;
    case KEYBOARD_LEFT:
        {
            index = (left_buttons_.size() - 1) >> 1;
            left_buttons_[index]->setFocus();
        }
        break;
    case KEYBOARD_RIGHT:
        {
            index = (right_buttons_.size() - 1) >> 1;
            right_buttons_[index]->setFocus();
        }
        break;
    case KEYBOARD_CENTER:
        {
            index = (center_buttons_.size() - 1) >> 1;
            center_buttons_[index]->setFocus();
        }
        break;
    default:
        break;
    }
}

}   // namespace ui

