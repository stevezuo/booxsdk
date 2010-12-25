#include "onyx/ui/russian_keyboard_layout.h"
#include "onyx/sys/sys.h"

namespace ui
{

RussianKeyboardLayout::RussianKeyboardLayout()
: KeyboardLayout()
{
    standard_key_size_ = QSize(43, 43);
    initCode();
    initShiftMap();
    initSpecialMaps();
    initKeys();
}

RussianKeyboardLayout::~RussianKeyboardLayout()
{
}

QSize RussianKeyboardLayout::getKeySize(int code)
{
    QChar ch(code);
    if (ch.isNumber())
    {
        return QSize(52, 52);
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
        int len = (standard_key_size_.width() * (has_touch ? 4 : 6)) + (has_touch ? 12 : 20);
        return QSize(len, standard_key_size_.height());
    }

    if (code == EnterCode)
    {
        return QSize(((standard_key_size_.width() << 1) + 4) , standard_key_size_.height());
    }

    if (code == BackSpace)
    {
        return QSize(((standard_key_size_.width() << 1) + 4), standard_key_size_.height());
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
        return QSize(((standard_key_size_.width() << 1) + 4), standard_key_size_.height());
    }

    if (code == HandWriting)
    {
        return QSize(((standard_key_size_.width() << 1) + 4), standard_key_size_.height());
    }

    if (code == SwitchLanguage)
    {
        return QSize(((standard_key_size_.width() << 1) + 4), standard_key_size_.height());
    }

    if (code == CapLock)
    {
        return QSize(((standard_key_size_.width() << 1) + 4), standard_key_size_.height());
    }

    return standard_key_size_;
}

void RussianKeyboardLayout::initCode()
{
    code_vector_.clear();
    code_vector_.push_back(QChar(0x0430));     // 0
    code_vector_.push_back(QChar(0x0431));     // 1
    code_vector_.push_back(QChar(0x0432));     // 2
    code_vector_.push_back(QChar(0x0433));     // 3
    code_vector_.push_back(QChar(0x0434));     // 4
    code_vector_.push_back(QChar(0x0435));     // 5
    code_vector_.push_back(QChar(0x0436));     // 6
    code_vector_.push_back(QChar(0x0437));     // 7
    code_vector_.push_back(QChar(0x0438));     // 8
    code_vector_.push_back(QChar(0x0439));     // 9
    code_vector_.push_back(QChar(0x043A));     // 10
    code_vector_.push_back(QChar(0x043B));     // 11
    code_vector_.push_back(QChar(0x043C));     // 12
    code_vector_.push_back(QChar(0x043D));     // 13
    code_vector_.push_back(QChar(0x043E));     // 14
    code_vector_.push_back(QChar(0x043F));     // 15
    code_vector_.push_back(QChar(0x0440));     // 16
    code_vector_.push_back(QChar(0x0441));     // 17
    code_vector_.push_back(QChar(0x0442));     // 18
    code_vector_.push_back(QChar(0x0443));     // 19
    code_vector_.push_back(QChar(0x0444));     // 20
    code_vector_.push_back(QChar(0x0445));     // 21
    code_vector_.push_back(QChar(0x0446));     // 22
    code_vector_.push_back(QChar(0x0447));     // 23
    code_vector_.push_back(QChar(0x0448));     // 24
    code_vector_.push_back(QChar(0x0449));     // 25
    code_vector_.push_back(QChar(0x044A));     // 26
    code_vector_.push_back(QChar(0x044B));     // 27
    code_vector_.push_back(QChar(0x044C));     // 28
    code_vector_.push_back(QChar(0x044D));     // 29
    code_vector_.push_back(QChar(0x044E));     // 30
    code_vector_.push_back(QChar(0x044F));     // 31
}

void RussianKeyboardLayout::initKeys()
{
    // resize keys
    keys_.clear();
    keys_.resize(5);
    int index = 0;

    // row 0
    keys_[0].resize(10);
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

    index = 0;
    keys_[1].resize(12);
    setKey(1, index++, code_vector_[9]);
    setKey(1, index++, code_vector_[22]);
    setKey(1, index++, code_vector_[19]);
    setKey(1, index++, code_vector_[10]);
    setKey(1, index++, code_vector_[5]);
    setKey(1, index++, code_vector_[13]);
    setKey(1, index++, code_vector_[3]);
    setKey(1, index++, code_vector_[24]);
    setKey(1, index++, code_vector_[25]);
    setKey(1, index++, code_vector_[7]);
    setKey(1, index++, code_vector_[21]);
    setKey(1, index++, code_vector_[26]);

    index = 0;
    keys_[2].resize(12);
    setKey(2, index++, code_vector_[20]);
    setKey(2, index++, code_vector_[27]);
    setKey(2, index++, code_vector_[2]);
    setKey(2, index++, code_vector_[0]);
    setKey(2, index++, code_vector_[15]);
    setKey(2, index++, code_vector_[16]);
    setKey(2, index++, code_vector_[14]);
    setKey(2, index++, code_vector_[11]);
    setKey(2, index++, code_vector_[4]);
    setKey(2, index++, code_vector_[6]);
    setKey(2, index++, code_vector_[29]);
    setKey(2, index++, code_vector_[30]);

    index = 0;
    keys_[3].resize(10);
    setKeyUnicode(3, index++, CapLock);
    setKey(3, index++, code_vector_[31]);
    setKey(3, index++, code_vector_[23]);
    setKey(3, index++, code_vector_[17]);
    setKey(3, index++, code_vector_[12]);
    setKey(3, index++, code_vector_[8]);
    setKey(3, index++, code_vector_[18]);
    setKey(3, index++, code_vector_[28]);
    setKey(3, index++, code_vector_[1]);
    setKeyUnicode(3, index++, BackSpace);

    index = 0;
    bool has_touch = SysStatus::instance().hasTouchScreen();
    keys_[4].resize(has_touch ? 5 : 4);
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
