#ifndef ONYX_RUSSIAN_KEYBOARD_LAYOUT_H_
#define ONYX_RUSSIAN_KEYBOARD_LAYOUT_H_

#include "keyboard_layout.h"

namespace ui
{

class RussianKeyboardLayout: public KeyboardLayout
{
public:
    RussianKeyboardLayout();
    virtual ~RussianKeyboardLayout();

    virtual QSize getKeySize(int code);

protected:
    virtual void initCode();
    virtual void initKeys();
};

};

#endif
