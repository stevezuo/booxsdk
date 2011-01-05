#include "onyx/ui/english_keyboard_layout.h"
#include "onyx/sys/sys.h"

namespace ui
{

EnglishKeyboardLayout::EnglishKeyboardLayout()
: KeyboardLayout()
{
    standard_key_size_ = QSize(44, 44);
    initCode();
    initShiftMap();
    initSpecialMaps();
    initKeys();
}

EnglishKeyboardLayout::~EnglishKeyboardLayout()
{
}

QSize EnglishKeyboardLayout::getKeySize(int code)
{
    QChar ch(code);

    if (ch.isNumber())
    {
        return standard_key_size_;
    }

    if (code < BSCode)
    {
        return standard_key_size_;
    }

    if (code > UnknownCode)
    {
        return standard_key_size_;
    }

    if (code == Blank)
    {
        bool has_touch = SysStatus::instance().hasTouchScreen();
        int len = standard_key_size_.width() * (has_touch ? 5 : 6) + (has_touch ? 16 : 20);
        return QSize(len, standard_key_size_.height());
    }

    if (code == EnterCode)
    {
        return QSize(((standard_key_size_.width() << 1) + 4) , standard_key_size_.height());
    }

    if (code == BackSpace)
    {
        return standard_key_size_;
    }

    if (code == BackSlash)
    {
        return standard_key_size_;
    }

    if (code == DeleteCode)
    {
        return standard_key_size_;
    }

    if (code == ShiftCode)
    {
        return standard_key_size_;
    }

    if (code == HandWriting)
    {
        return standard_key_size_;
    }

    if (code == SwitchLanguage)
    {
        return standard_key_size_;
    }

    if (code == CapLock)
    {
        return standard_key_size_;
    }

    return standard_key_size_;
}

void EnglishKeyboardLayout::initCode()
{
    code_vector_.clear();
    code_vector_.push_back(QChar('a'));     // 0
    code_vector_.push_back(QChar('b'));     // 1
    code_vector_.push_back(QChar('c'));     // 2
    code_vector_.push_back(QChar('d'));     // 3
    code_vector_.push_back(QChar('e'));     // 4
    code_vector_.push_back(QChar('f'));     // 5
    code_vector_.push_back(QChar('g'));     // 6
    code_vector_.push_back(QChar('h'));     // 7
    code_vector_.push_back(QChar('i'));     // 8
    code_vector_.push_back(QChar('j'));     // 9
    code_vector_.push_back(QChar('k'));     // 10
    code_vector_.push_back(QChar('l'));     // 11
    code_vector_.push_back(QChar('m'));     // 12
    code_vector_.push_back(QChar('n'));     // 13
    code_vector_.push_back(QChar('o'));     // 14
    code_vector_.push_back(QChar('p'));     // 15
    code_vector_.push_back(QChar('q'));     // 16
    code_vector_.push_back(QChar('r'));     // 17
    code_vector_.push_back(QChar('s'));     // 18
    code_vector_.push_back(QChar('t'));     // 19
    code_vector_.push_back(QChar('u'));     // 20
    code_vector_.push_back(QChar('v'));     // 21
    code_vector_.push_back(QChar('w'));     // 22
    code_vector_.push_back(QChar('x'));     // 23
    code_vector_.push_back(QChar('y'));     // 24
    code_vector_.push_back(QChar('z'));     // 25
    code_vector_.push_back(QChar('/'));     // 26
    code_vector_.push_back(QChar(':'));     // 27
    code_vector_.push_back(QChar('"'));     // 28
    code_vector_.push_back(QChar(','));     // 29
    code_vector_.push_back(QChar('.'));     // 30
    code_vector_.push_back(QChar('?'));     // 31
    code_vector_.push_back(QChar('!'));     // 32
}

void EnglishKeyboardLayout::initKeys()
{
    // resize keys
    keys_.clear();
    keys_.resize(5);
    int index = 0;

    // row 0
    keys_[0].resize(11);
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
    setKeyUnicode(0, index++, BackSpace);

    index = 0;
    keys_[1].resize(11);
    setKey(1, index++, code_vector_[16]);
    setKey(1, index++, code_vector_[22]);
    setKey(1, index++, code_vector_[4]);
    setKey(1, index++, code_vector_[17]);
    setKey(1, index++, code_vector_[19]);
    setKey(1, index++, code_vector_[24]);
    setKey(1, index++, code_vector_[20]);
    setKey(1, index++, code_vector_[8]);
    setKey(1, index++, code_vector_[14]);
    setKey(1, index++, code_vector_[15]);
    setKey(1, index++, code_vector_[26]);

    index = 0;
    keys_[2].resize(11);
    setKey(2, index++, code_vector_[0]);
    setKey(2, index++, code_vector_[18]);
    setKey(2, index++, code_vector_[3]);
    setKey(2, index++, code_vector_[5]);
    setKey(2, index++, code_vector_[6]);
    setKey(2, index++, code_vector_[7]);
    setKey(2, index++, code_vector_[9]);
    setKey(2, index++, code_vector_[10]);
    setKey(2, index++, code_vector_[11]);
    setKey(2, index++, code_vector_[27]);
    setKey(2, index++, code_vector_[28]);

    index = 0;
    keys_[3].resize(11);
    setKey(3, index++, code_vector_[25]);
    setKey(3, index++, code_vector_[23]);
    setKey(3, index++, code_vector_[2]);
    setKey(3, index++, code_vector_[21]);
    setKey(3, index++, code_vector_[1]);
    setKey(3, index++, code_vector_[13]);
    setKey(3, index++, code_vector_[12]);
    setKey(3, index++, code_vector_[29]);
    setKey(3, index++, code_vector_[30]);
    setKey(3, index++, code_vector_[31]);
    setKey(3, index++, code_vector_[32]);

    index = 0;
    bool has_touch = SysStatus::instance().hasTouchScreen();
    keys_[4].resize(has_touch ? 6 : 5);
    setKeyUnicode(4, index++, CapLock);
    setKeyUnicode(4, index++, ShiftCode);
    setKeyUnicode(4, index++, Blank);
    setKeyUnicode(4, index++, EnterCode);
    setKeyUnicode(4, index++, SwitchLanguage);
    if (has_touch)
    {
        setKeyUnicode(4, index++, HandWriting);
    }
}

}
