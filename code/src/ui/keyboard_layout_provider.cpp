#include "onyx/ui/keyboard_layout_provider.h"
#include "onyx/ui/english_keyboard_layout.h"
#include "onyx/ui/russian_keyboard_layout.h"

namespace ui
{

KeyboardLayoutProvider::KeyboardLayoutProvider()
: locale_(QLocale::system())
{
}

KeyboardLayoutProvider::~KeyboardLayoutProvider()
{
}

KeyboardLayout* KeyboardLayoutProvider::getLayout(const QLocale & locale)
{
    locale_ = locale;
    if (isRussian(locale_))
    {
        // return russian layout
        return new RussianKeyboardLayout();
    }
    else if (isSwedish(locale_))
    {
        // return swedish layout
    }
    else if (isPolish(locale_))
    {
        // return polish layout
    }
    else if (isHungarian(locale_))
    {
        // return hungarian layout
    }

    return new EnglishKeyboardLayout();
}

bool KeyboardLayoutProvider::isRussian(const QLocale & locale)
{
    return (locale.language() == QLocale::Russian ||
            locale.country()  == QLocale::RussianFederation);
}

bool KeyboardLayoutProvider::isSwedish(const QLocale & locale)
{
    return (locale.language() == QLocale::Swedish ||
            locale.country()  == QLocale::Sweden);
}

bool KeyboardLayoutProvider::isPolish(const QLocale & locale)
{
    return (locale.language() == QLocale::Polish ||
            locale.country()  == QLocale::Poland);
}

bool KeyboardLayoutProvider::isHungarian(const QLocale & locale)
{
    return (locale.language() == QLocale::Hungarian ||
            locale.country()  == QLocale::Hungary);
}

}
