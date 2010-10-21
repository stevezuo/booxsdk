#include "onyx/sys/sys.h"
#include "onyx/screen/screen_proxy.h"
#include "onyx/data/sketch_proxy.h"
#include "onyx/ui/keyboard.h"
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
static bool is_english = true;
bool isRussian()
{
    return (currentLanguage() == QLocale::Russian ||
            currentCountry()  == QLocale::RussianFederation);
}

bool isSwedish()
{
    return (currentLanguage() == QLocale::Swedish ||
            currentCountry()  == QLocale::Sweden);
}
bool isEnglishLayout()
{
    return is_english;
}

int standardKeySize()
{
    if (isRussian() && !isEnglishLayout())
    {
        return 43;
    }
    else if (isSwedish())
    {
        return 45;
    }       
    return 52;
}

QVector<QChar> code_vector;
void initRussianCode(QVector<QChar> & code_vector)
{
    code_vector.push_back(QChar(0x0430));     // 0
    code_vector.push_back(QChar(0x0431));     // 1
    code_vector.push_back(QChar(0x0432));     // 2
    code_vector.push_back(QChar(0x0433));     // 3
    code_vector.push_back(QChar(0x0434));     // 4
    code_vector.push_back(QChar(0x0435));     // 5
    code_vector.push_back(QChar(0x0436));     // 6
    code_vector.push_back(QChar(0x0437));     // 7
    code_vector.push_back(QChar(0x0438));     // 8
    code_vector.push_back(QChar(0x0439));     // 9
    code_vector.push_back(QChar(0x043A));     // 10
    code_vector.push_back(QChar(0x043B));     // 11
    code_vector.push_back(QChar(0x043C));     // 12
    code_vector.push_back(QChar(0x043D));     // 13
    code_vector.push_back(QChar(0x043E));     // 14
    code_vector.push_back(QChar(0x043F));     // 15
    code_vector.push_back(QChar(0x0440));     // 16
    code_vector.push_back(QChar(0x0441));     // 17
    code_vector.push_back(QChar(0x0442));     // 18
    code_vector.push_back(QChar(0x0443));     // 19
    code_vector.push_back(QChar(0x0444));     // 20
    code_vector.push_back(QChar(0x0445));     // 21
    code_vector.push_back(QChar(0x0446));     // 22
    code_vector.push_back(QChar(0x0447));     // 23
    code_vector.push_back(QChar(0x0448));     // 24
    code_vector.push_back(QChar(0x0449));     // 25
    code_vector.push_back(QChar(0x044A));     // 26
    code_vector.push_back(QChar(0x044B));     // 27
    code_vector.push_back(QChar(0x044C));     // 28
    code_vector.push_back(QChar(0x044D));     // 29
    code_vector.push_back(QChar(0x044E));     // 30
    code_vector.push_back(QChar(0x044F));     // 31
}

void initEnglishCode(QVector<QChar> & code_vector)
{
    code_vector.push_back(QChar('a'));     // 0
    code_vector.push_back(QChar('b'));     // 1
    code_vector.push_back(QChar('c'));     // 2
    code_vector.push_back(QChar('d'));     // 3
    code_vector.push_back(QChar('e'));     // 4
    code_vector.push_back(QChar('f'));     // 5
    code_vector.push_back(QChar('g'));     // 6
    code_vector.push_back(QChar('h'));     // 7
    code_vector.push_back(QChar('i'));     // 8
    code_vector.push_back(QChar('j'));     // 9
    code_vector.push_back(QChar('k'));     // 10
    code_vector.push_back(QChar('l'));     // 11
    code_vector.push_back(QChar('m'));     // 12
    code_vector.push_back(QChar('n'));     // 13
    code_vector.push_back(QChar('o'));     // 14
    code_vector.push_back(QChar('p'));     // 15
    code_vector.push_back(QChar('q'));     // 16
    code_vector.push_back(QChar('r'));     // 17
    code_vector.push_back(QChar('s'));     // 18
    code_vector.push_back(QChar('t'));     // 19
    code_vector.push_back(QChar('u'));     // 20
    code_vector.push_back(QChar('v'));     // 21
    code_vector.push_back(QChar('w'));     // 22
    code_vector.push_back(QChar('x'));     // 23
    code_vector.push_back(QChar('y'));     // 24
    code_vector.push_back(QChar('z'));     // 25
}

void initCodeVector()
{
    code_vector.clear();
    if (isRussian() && !isEnglishLayout())
    {
        initRussianCode(code_vector);
    }
    else
    {
        initEnglishCode(code_vector);
    }
}

ShiftMap shift_map;
void initShiftMap()
{
    assert(!code_vector.isEmpty());
    if (!shift_map.isEmpty())
    {
        shift_map.clear();
    }

    shift_map['1'] = '!';
    shift_map['2'] = '@';
    shift_map['3'] = '#';
    shift_map['4'] = '$';
    shift_map['5'] = '%';
    shift_map['6'] = '^';
    shift_map['7'] = QChar(0x0026);
    shift_map['8'] = '*';
    shift_map['9'] = '(';
    shift_map['0'] = ')';
    shift_map[code_vector[16]] = '~';
    shift_map[code_vector[22]] = '`';
    shift_map[code_vector[4]] = '-';
    shift_map[code_vector[17]] = '_';
    shift_map[code_vector[19]] = '+';
    shift_map[code_vector[24]] = '=';
    shift_map[code_vector[20]] = '{';
    shift_map[code_vector[8]] = '}';
    shift_map[code_vector[14]] = '[';
    shift_map[code_vector[15]] = ']';
    shift_map[code_vector[0]] = ':';
    shift_map[code_vector[18]] = ';';
    shift_map[code_vector[3]] = '"';
    shift_map[code_vector[5]] = '\'';
    shift_map[code_vector[6]] = '<';
    shift_map[code_vector[7]] = '>';
    shift_map[code_vector[9]] = ',';
    shift_map[code_vector[10]] = '|';
    shift_map[code_vector[11]] = '?';
    shift_map[code_vector[25]] = '\\';
    shift_map[code_vector[23]] = '.';
    shift_map[code_vector[2]] = '/';
    shift_map[code_vector[21]] = QChar(0x00A3);//£
    shift_map[code_vector[1]]  = QChar(0x00A5);//￥
    shift_map[code_vector[13]] = QChar(0x00A7);//§
    shift_map[code_vector[12]] = QChar(0x00A9);//©

    if (code_vector.size() > 26)
    {
        int offset = code_vector.size() - 26;
        for (int i = 1; i <= offset; ++i)
        {
            shift_map[code_vector[25 + i]] = QChar(0x00A9 + (i << 1));
        }
    }
}

SpecialMaps special_maps;
void initSpecialMaps()
{
    if (!special_maps.isEmpty())
    {
        special_maps.clear();
    }
    special_maps.push_back(SpecialMap(Qt::Key_Backspace, 8, "", "backspace", 0));
    special_maps.push_back(SpecialMap(Qt::Key_Tab, 9, "Tab", NULL, 0));
    special_maps.push_back(SpecialMap(Qt::Key_CapsLock, 0xffff, "Caps", NULL, 0));
    special_maps.push_back(SpecialMap(Qt::Key_Shift, 0xffff, "Shift", NULL, 0));
    special_maps.push_back(SpecialMap(Qt::Key_Space, ' ', "Space", NULL, 0));
    special_maps.push_back(SpecialMap(BackSlash, 43,     "\\",    NULL, 0));
    special_maps.push_back(SpecialMap(Qt::Key_Left, 0xffff, "<",     "leftarrow", 0));
    special_maps.push_back(SpecialMap(Qt::Key_Right, 0xffff, ">",     "rightarrow", 0));
    special_maps.push_back(SpecialMap(Qt::Key_Insert, 0xffff, "I",     "insert", 0));
    special_maps.push_back(SpecialMap(Qt::Key_Home, 0xffff, "H",     "home", 0));
    special_maps.push_back(SpecialMap(Qt::Key_End, 0xffff, "E",     "end", 0));
    special_maps.push_back(SpecialMap(Qt::Key_Delete, 0xffff, "X",     "delete", 0));
    special_maps.push_back(SpecialMap(Blank, 0, "Space", 0, 0));
    special_maps.push_back(SpecialMap(Qt::Key_Enter, 0xffff, "Enter", NULL, 0));

    QString switch_lan_title;
    if (isRussian() && !isEnglishLayout())
    {
        switch_lan_title = "abc";
    }
    else
    {
        switch_lan_title.append(QChar(0x0430));
        switch_lan_title.append(QChar(0x0431));
        switch_lan_title.append(QChar(0x0432));
    }
    special_maps.push_back(SpecialMap(SwitchLanguage, 0xffff, switch_lan_title, 0, 0));
    special_maps.push_back(SpecialMap(HandWriting, 0xffff, QApplication::tr("Write"), 0, 0));
    special_maps.push_back(SpecialMap(0, 0, 0, 0, 0));
}

static QVector<uint> keyboard_standard[5];
void setKey(int row, int idx, QChar c)
{
    keyboard_standard[row][idx] = c.unicode();
}

void setKeyUnicode(int row, int idx, uint c)
{
    keyboard_standard[row][idx] = c;
}

void initEnglishKeyboard()
{
    int index = 0;
    if (!keyboard_standard[0].isEmpty())
    {
        keyboard_standard[0].clear();
    }
    keyboard_standard[0].resize(isSwedish()?11:10);
    setKey(0, index++, '1');
    setKey(0, index++, '2');
    setKey(0, index++, '3');
    setKey(0, index++, '4');
    setKey(0, index++, '5');
    setKey(0, index++, '6');
    setKey(0, index++, '7');
    setKey(0, index++, '8');
    setKey(0, index++, '9');
    setKey(0, index++, '0');
    if (isSwedish())
    setKey(0, index++, QChar(0x00E5));

    if (!keyboard_standard[1].isEmpty())
    {
        keyboard_standard[1].clear();
    }
    keyboard_standard[1].resize(isSwedish()?11:10);
    index = 0;
    setKey(1, index++, code_vector[16]);
    setKey(1, index++, code_vector[22]);
    setKey(1, index++, code_vector[4]);
    setKey(1, index++, code_vector[17]);
    setKey(1, index++, code_vector[19]);
    setKey(1, index++, code_vector[24]);
    setKey(1, index++, code_vector[20]);
    setKey(1, index++, code_vector[8]);
    setKey(1, index++, code_vector[14]);
    setKey(1, index++, code_vector[15]);
    if (isSwedish())
    setKey(1, index++, QChar(0x00E4));

    if (!keyboard_standard[2].isEmpty())
    {
        keyboard_standard[2].clear();
    }
    keyboard_standard[2].resize(isSwedish()?11:10);
    index = 0;
    setKey(2, index++, code_vector[0]);
    setKey(2, index++, code_vector[18]);
    setKey(2, index++, code_vector[3]);
    setKey(2, index++, code_vector[5]);
    setKey(2, index++, code_vector[6]);
    setKey(2, index++, code_vector[7]);
    setKey(2, index++, code_vector[9]);
    setKey(2, index++, code_vector[10]);
    setKey(2, index++, code_vector[11]);
    setKey(2, index++, code_vector[12]);
    if (isSwedish())
    setKey(2, index++, QChar(0x00F6));

    if (!keyboard_standard[3].isEmpty())
    {
        keyboard_standard[3].clear();
    }
    keyboard_standard[3].resize(8);
    index = 0;
    setKeyUnicode(3, index++, CapLock);
    setKey(3, index++, code_vector[25]);
    setKey(3, index++, code_vector[23]);
    setKey(3, index++, code_vector[2]);
    setKey(3, index++, code_vector[21]);
    setKey(3, index++, code_vector[1]);
    setKey(3, index++, code_vector[13]);
    setKeyUnicode(3, index++, BackSpace);

    if (!keyboard_standard[4].isEmpty())
    {
        keyboard_standard[4].clear();
    }
    bool is_special_locale = (currentLanguage() == QLocale::Russian ||
                              currentCountry()  == QLocale::RussianFederation);
    bool has_touch = SysStatus::instance().hasTouchScreen();
    keyboard_standard[4].resize(is_special_locale ? (has_touch ? 5 : 4) : (has_touch ? 4 : 3));

    index = 0;
    setKeyUnicode(4, index++, ShiftCode);
    setKeyUnicode(4, index++, Blank);
    setKeyUnicode(4, index++, EnterCode);
    if (has_touch)
    {
        setKeyUnicode(4, index++, HandWriting);
    }
    if (is_special_locale)
    {
        setKeyUnicode(4, index++, SwitchLanguage);
    }
}

void initRussianKeyboard()
{
    int index = 0;
    if (!keyboard_standard[0].isEmpty())
    {
        keyboard_standard[0].clear();
    }
    keyboard_standard[0].resize(10);
    setKey(0, index++, '1');
    setKey(0, index++, '2');
    setKey(0, index++, '3');
    setKey(0, index++, '4');
    setKey(0, index++, '5');
    setKey(0, index++, '6');
    setKey(0, index++, '7');
    setKey(0, index++, '8');
    setKey(0, index++, '9');
    setKey(0, index++, '0');

    if (!keyboard_standard[1].isEmpty())
    {
        keyboard_standard[1].clear();
    }
    keyboard_standard[1].resize(12);
    index = 0;
    setKey(1, index++, code_vector[9]);
    setKey(1, index++, code_vector[22]);
    setKey(1, index++, code_vector[19]);
    setKey(1, index++, code_vector[10]);
    setKey(1, index++, code_vector[5]);
    setKey(1, index++, code_vector[13]);
    setKey(1, index++, code_vector[3]);
    setKey(1, index++, code_vector[24]);
    setKey(1, index++, code_vector[25]);
    setKey(1, index++, code_vector[7]);
    setKey(1, index++, code_vector[21]);
    setKey(1, index++, code_vector[26]);

    if (!keyboard_standard[2].isEmpty())
    {
        keyboard_standard[2].clear();
    }
    keyboard_standard[2].resize(12);
    index = 0;
    setKey(2, index++, code_vector[20]);
    setKey(2, index++, code_vector[27]);
    setKey(2, index++, code_vector[2]);
    setKey(2, index++, code_vector[0]);
    setKey(2, index++, code_vector[15]);
    setKey(2, index++, code_vector[16]);
    setKey(2, index++, code_vector[14]);
    setKey(2, index++, code_vector[11]);
    setKey(2, index++, code_vector[4]);
    setKey(2, index++, code_vector[6]);
    setKey(2, index++, code_vector[29]);
    setKey(2, index++, code_vector[30]);

    if (!keyboard_standard[3].isEmpty())
    {
        keyboard_standard[3].clear();
    }
    keyboard_standard[3].resize(10);
    index = 0;
    setKeyUnicode(3, index++, CapLock);
    setKey(3, index++, code_vector[31]);
    setKey(3, index++, code_vector[23]);
    setKey(3, index++, code_vector[17]);
    setKey(3, index++, code_vector[12]);
    setKey(3, index++, code_vector[8]);
    setKey(3, index++, code_vector[18]);
    setKey(3, index++, code_vector[28]);
    setKey(3, index++, code_vector[1]);
    setKeyUnicode(3, index++, BackSpace);

    if (!keyboard_standard[4].isEmpty())
    {
        keyboard_standard[4].clear();
    }
    bool is_special_locale = (currentLanguage() == QLocale::Russian ||
                              currentCountry()  == QLocale::RussianFederation);
    bool has_touch = SysStatus::instance().hasTouchScreen();
    keyboard_standard[4].resize(is_special_locale ? (has_touch ? 5 : 4) : (has_touch ? 4 : 3));

    index = 0;
    setKeyUnicode(4, index++, ShiftCode);
    setKeyUnicode(4, index++, Blank);
    setKeyUnicode(4, index++, EnterCode);
    if (has_touch)
    {
        setKeyUnicode(4, index++, HandWriting);
    }
    if (is_special_locale)
    {
        setKeyUnicode(4, index++, SwitchLanguage);
    }
}

int getQKeyCode(uint code)
{
    int ret = code;
    if (code > BSCode && code < UnknownCode)
    {
        int code_index = code - BSCode - 1;
        if (code_index >= 0 && code_index < special_maps.size())
        {
            ret = special_maps[code_index].qcode;
        }
    }
    return ret;
}

KeyBoard::KeyBoard(QWidget* parent, Qt::WFlags f)
    : QFrame(parent, f)
    , shift_(0)
    , lock_(0)
    , is_handwriting_(0)
    , ver_layout_(0)
    , button_group_(0)
    , receiver_(0)
    , sketch_proxy_(0)
    , finish_character_timer_(finish_charater_interval, this, SLOT(handleFinishCharacterTimeOut()))
    , auto_select_timer_(auto_select_interval, this, SLOT(handleAutoSelect()))
{
    is_english = !isRussian();
    init();
}

KeyBoard::~KeyBoard()
{
}

void KeyBoard::resetState()
{
}

void KeyBoard::init()
{
    shift_ = 0;
    lock_  = 0;
    initCodeVector();
    initShiftMap();
    initSpecialMaps();

    if (isRussian() && !isEnglishLayout())
    {
        initRussianKeyboard();
    }
    else
    {
        initEnglishKeyboard();
    }

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
    for (int i = 0; i < 5; ++i)
    {
        shared_ptr<QHBoxLayout> hor_layout(new QHBoxLayout());
        hor_layouts_.push_back(hor_layout);
        hor_layout->setAlignment(Qt::AlignCenter);
        hor_layout->setSpacing(4);
        hor_layout->setContentsMargins(0, 0, 0, 0);
        for (int k = 0; k < keyboard_standard[i].size(); ++k)
        {
            shared_ptr<KeyBoardKey> key(new KeyBoardKey(this));
            key->setCode(keyboard_standard[i][k]);

            // connect the signals
            connect(this,
                    SIGNAL(shifted(bool)),
                    key.get(),
                    SLOT(handleShifted(bool)));
            connect(this,
                    SIGNAL(capLocked(bool)),
                    key.get(),
                    SLOT(handleCapLocked(bool)));

            hor_layout->addWidget(key.get());
            button_group_->addButton(key.get());
            buttons_.push_back(key);
        }
        ver_layout_->addLayout(hor_layout.get());
    }

    im_char_selection_.reset(new InputMethodCharSelection(this));
    ver_layout_->addWidget(im_char_selection_.get());
    im_char_selection_->setVisible(false);
    connect(im_char_selection_.get(),
            SIGNAL(textSelected(const QString&, int)),
            this,
            SLOT(handleTextSelected(const QString&, int)),
            Qt::QueuedConnection);
    connect(im_char_selection_.get(),
            SIGNAL(buttonClicked(QAbstractButton *)),
            this,
            SLOT(handleButtonClicked(QAbstractButton *)),
            Qt::QueuedConnection);

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
            this, SLOT(handleHandwritingFunctionClicked(const QModelIndex &)));

    connect(button_group_.get(),
            SIGNAL(buttonClicked(QAbstractButton*)),
            this,
            SLOT(handleButtonClicked(QAbstractButton*)),
            Qt::QueuedConnection);
}

void KeyBoard::keyReleaseEvent(QKeyEvent *e)
{
    e->ignore();
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
    is_english = !is_english;
    hide();
    init();
    show();
    onyx::screen::instance().flush(0, onyx::screen::ScreenProxy::GC);
}

void KeyBoard::handleHandwritingFunctionClicked(const QModelIndex & index)
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

void KeyBoard::handleStrokeStarted()
{
    auto_select_timer_.stop();
    finish_character_timer_.stop();
}

void KeyBoard::handlePointAdded(SketchPoint point)
{
    // collect point
    HandwritingManager::instance().collectPoint(point.x(), point.y());
}

void KeyBoard::handleStrokeAdded(const Points & points)
{
    // finish stroke
    HandwritingManager::instance().finishStroke();

    // start to check whether finishing the character
    finish_character_timer_.start();
}

void KeyBoard::handleFinishCharacterTimeOut()
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

void KeyBoard::handleTextSelected(const QString & text, int index)
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

void KeyBoard::handleAutoSelect()
{
    if (!candidates_.isEmpty())
    {
        QString best = candidates_.at(0);
        handleTextSelected(best, 0);
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

    int key_code = getQKeyCode(code);
    QString text = getRealChar(code);

    // Post the key event to receiver
    QKeyEvent * key_event = new QKeyEvent(type, key_code, modifiers_, text);
    QApplication::postEvent(receiver_, key_event);
}

void KeyBoard::handleButtonClicked(QAbstractButton *button)
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
}

}   // namespace ui

