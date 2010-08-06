// -*- mode: c++; c-basic-offset: 4; -*-

#ifndef ONYX_BASE_QT_SUPPORT_H__
#define ONYX_BASE_QT_SUPPORT_H__

#include <iostream>
#include <QString>

std::ostream& operator<<(std::ostream& ost, const QString& qstr) {
    return (ost << qstr.toStdString());
}

#endif  // ONYX_BASE_QT_SUPPORT_H__
