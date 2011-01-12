#include "onyx/ui/keyboard_layout.h"

namespace ui
{
KeyboardLayout::KeyboardLayout()
{
}

KeyboardLayout::~KeyboardLayout()
{
}

void KeyboardLayout::initShiftMap()
{
    assert(!code_vector_.isEmpty());

    shift_map_.clear();
    shift_map_['1'] = '!';
    shift_map_['2'] = '@';
    shift_map_['3'] = '#';
    shift_map_['4'] = '$';
    shift_map_['5'] = '%';
    shift_map_['6'] = '^';
    shift_map_['7'] = QChar(0x0026);
    shift_map_['8'] = '*';
    shift_map_['9'] = '(';
    shift_map_['0'] = ')';
    shift_map_[code_vector_[16]] = '~';
    shift_map_[code_vector_[22]] = '`';
    shift_map_[code_vector_[4]] = '-';
    shift_map_[code_vector_[17]] = '_';
    shift_map_[code_vector_[19]] = '+';
    shift_map_[code_vector_[24]] = '=';
    shift_map_[code_vector_[20]] = '{';
    shift_map_[code_vector_[8]] = '}';
    shift_map_[code_vector_[14]] = '[';
    shift_map_[code_vector_[15]] = ']';
    shift_map_[code_vector_[0]] = ':';
    shift_map_[code_vector_[18]] = ';';
    shift_map_[code_vector_[3]] = '"';
    shift_map_[code_vector_[5]] = '\'';
    shift_map_[code_vector_[6]] = '<';
    shift_map_[code_vector_[7]] = '>';
    shift_map_[code_vector_[9]] = ',';
    shift_map_[code_vector_[10]] = '|';
    shift_map_[code_vector_[11]] = '?';
    shift_map_[code_vector_[25]] = '\\';
    shift_map_[code_vector_[23]] = '.';
    shift_map_[code_vector_[2]] = '/';
    shift_map_[code_vector_[21]] = QChar(0x00A3);
    shift_map_[code_vector_[1]]  = QChar(0x00A5);
    shift_map_[code_vector_[13]] = QChar(0x00A7);
    shift_map_[code_vector_[12]] = QChar(0x00A9);

    if (code_vector_.size() > 26)
    {
        int offset = code_vector_.size() - 26;
        for (int i = 1; i <= offset; ++i)
        {
            shift_map_[code_vector_[25 + i]] = QChar(0x00A9 + (i << 1));
        }
    }
}

void KeyboardLayout::initSpecialMaps()
{
    if (!special_maps_.isEmpty())
    {
        special_maps_.clear();
    }
    special_maps_.push_back(SpecialMap(Qt::Key_Backspace, 8, "", "backspace", 0));
    special_maps_.push_back(SpecialMap(Qt::Key_Tab, 9, "Tab", NULL, 0));
    special_maps_.push_back(SpecialMap(Qt::Key_CapsLock, 0xffff, "Caps", NULL, 0));
    special_maps_.push_back(SpecialMap(Qt::Key_Shift, 0xffff, "Shift", NULL, 0));
    special_maps_.push_back(SpecialMap(Qt::Key_Space, ' ', "Space", NULL, 0));
    special_maps_.push_back(SpecialMap(BackSlash, 43,     "\\",    NULL, 0));
    special_maps_.push_back(SpecialMap(Qt::Key_Left, 0xffff, "<",     "leftarrow", 0));
    special_maps_.push_back(SpecialMap(Qt::Key_Right, 0xffff, ">",     "rightarrow", 0));
    special_maps_.push_back(SpecialMap(Qt::Key_Insert, 0xffff, "I",     "insert", 0));
    special_maps_.push_back(SpecialMap(Qt::Key_Home, 0xffff, "H",     "home", 0));
    special_maps_.push_back(SpecialMap(Qt::Key_End, 0xffff, "E",     "end", 0));
    special_maps_.push_back(SpecialMap(Qt::Key_Delete, 0xffff, "X",     "delete", 0));
    special_maps_.push_back(SpecialMap(Blank, 0, "Space", 0, 0));
    special_maps_.push_back(SpecialMap(Qt::Key_Enter, 0xffff, "Enter", NULL, 0));

    special_maps_.push_back(SpecialMap(SwitchLanguage, 0xffff, QApplication::tr("abc"), 0, 0));
    special_maps_.push_back(SpecialMap(HandWriting, 0xffff, QApplication::tr("Write"), 0, 0));
    special_maps_.push_back(SpecialMap(0, 0, 0, 0, 0));
}

void KeyboardLayout::setKey(int row, int idx, QChar c)
{
    keys_[row][idx] = c.unicode();
}

void KeyboardLayout::setKeyUnicode(int row, int idx, uint c)
{
    keys_[row][idx] = c;
}

int KeyboardLayout::getQKeyCode(uint code)
{
    int ret = code;
    if (code > BSCode && code < UnknownCode)
    {
        int code_index = code - BSCode - 1;
        if (code_index >= 0 && code_index < special_maps_.size())
        {
            ret = special_maps_[code_index].qcode;
        }
    }
    return ret;
}

}
