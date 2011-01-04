#ifndef ONYX_KEYBOARD_LAYOUT_H_
#define ONYX_KEYBOARD_LAYOUT_H_

#include "keyboard_utils.h"

namespace ui
{

class KeyboardLayout
{
public:
    KeyboardLayout();
    virtual ~KeyboardLayout();

    virtual QSize getKeySize(int code) = 0;

    inline QVector<QChar> & codeVector() { return code_vector_; }
    inline SpecialMaps & specialMaps() { return special_maps_; }
    inline ShiftMap & shiftMap() { return shift_map_; }
    inline Keys & keys() { return keys_; }

    inline QSize getStandardKeySize() const { return standard_key_size_; }
    int getQKeyCode(uint code);

protected:
    virtual void initCode() = 0;
    virtual void initKeys() = 0;

    virtual void initShiftMap();
    virtual void initSpecialMaps();

    void setKey(int row, int idx, QChar c);
    void setKeyUnicode(int row, int idx, uint c);

protected:
    QVector<QChar> code_vector_;
    SpecialMaps    special_maps_;
    ShiftMap       shift_map_;
    Keys           keys_;
    QSize          standard_key_size_;
};

};

#endif
