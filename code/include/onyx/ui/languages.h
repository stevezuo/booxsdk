// Author: John

/// This is the main header file for the UI package. The ui namespace
/// encapsulate the classes we use in our user-facing applications.

#ifndef BASE_UI_LANGUAGE_H__
#define BASE_UI_LANGUAGE_H__

#include <Qt/QtCore>
#include <Qt/QtGui>


namespace ui
{

bool loadTranslator(const QString &locale);


}  // namespace ui

#endif  // LIB_UI_UI_H__
