#ifndef ONYX_ENGLISH_KEYBOARD_LAYOUT_H_
#define ONYX_ENGLISH_KEYBOARD_LAYOUT_H_

#include "keyboard_layout.h"

namespace ui
{

class EnglishKeyboardLayout: public KeyboardLayout
{
public:
    EnglishKeyboardLayout();
    virtual ~EnglishKeyboardLayout();

    virtual QSize getKeySize(int code);

protected:
    virtual void initCode();
    virtual void initKeys();
};

};

#endif
