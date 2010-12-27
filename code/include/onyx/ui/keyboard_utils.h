#ifndef ONYX_KEYBOARD_UTILS_H_
#define ONYX_KEYBOARD_UTILS_H_

#include "onyx/base/base.h"
#include "onyx/ui/ui_global.h"

namespace ui
{

enum
{
    BSCode = 0x80,
    BackSpace,
    TabCode,
    CapLock,
    ShiftCode,
    SpaceCode,
    BackSlash,
    LeftCode,
    RightCode,
    InsertCode,
    HomeCode,
    EndCode,
    DeleteCode,
    Blank,
    EnterCode,
    SwitchLanguage,
    HandWriting,
    UnknownCode
};

struct SpecialMap {
    int qcode;
    ushort unicode;
    QString label;
    QString pic_name;
    QPixmap *pic;

    SpecialMap() : qcode(0), unicode(0), pic(0) {}
    SpecialMap(int qc,
               ushort uc,
               const QString & l,
               const QString & pic,
               QPixmap * p)
               : qcode(qc), unicode(uc), label(l), pic_name(pic), pic(p) {}
    SpecialMap(const SpecialMap & right)
        : qcode(right.qcode), unicode(right.unicode), label(right.label)
        , pic_name(right.pic_name), pic(right.pic) {}
};

typedef QMap<QChar, QChar> ShiftMap;
typedef ShiftMap::iterator ShiftIter;

typedef QVector<SpecialMap> SpecialMaps;
typedef SpecialMaps::iterator SpecialMapIter;

typedef QVector< QVector<uint> > Keys;

int standardKeySize();
bool isRussian();
bool isSwedish();
bool isPolish();
bool isHungarian();
bool isEnglishLayout();

}   // namespace ui

#endif  // ONYX_KEYBOARD_UTILS_H_
