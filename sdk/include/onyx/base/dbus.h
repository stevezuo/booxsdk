// Author: John

/// This is the main header file for the DBus package.
/// Not all application use the DBus, so it's necessary to
/// make a separated file for DBus. Should include this file
/// instead of including QtDBus direclty, otherwise you will
/// have compiling warnings.

#ifndef BASE_DBUS_H__
#define BASE_DBUS_H__

#ifndef _WIN32
#pragma GCC system_header
#endif

#include <QtDBus/QtDBus>


#endif  // BASE_DBUS_H__
