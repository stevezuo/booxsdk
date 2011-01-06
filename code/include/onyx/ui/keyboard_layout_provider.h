#ifndef ONYX_KEYBOARD_LAYOUT_PROVIDER_H_
#define ONYX_KEYBOARD_LAYOUT_PROVIDER_H_

#include "keyboard_layout.h"

namespace ui
{

class KeyboardLayoutProvider
{
public:
    KeyboardLayoutProvider();
    ~KeyboardLayoutProvider();

    KeyboardLayout* getLayout(const QLocale & locale);
    inline const QLocale & currentLocale() const { return locale_; }

private:
    bool isRussian(const QLocale & locale);
    bool isSwedish(const QLocale & locale);
    bool isPolish(const QLocale & locale);
    bool isHungarian(const QLocale & locale);

private:
    QLocale locale_;
};

};
#endif
