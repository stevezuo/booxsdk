
#include "platform.h"
#include <QtCore/qglobal.h>

namespace sys
{

QString platform()
{
    static QString p;
    if (p.isEmpty())
    {
        p = qgetenv("PLATFORM");
        if (p.isEmpty())
        {
            p = "freescale";
        }
    }
    return p;
}


int defaultRotation()
{
    static int rotation = -1;
    if (rotation < 0)
    {
        rotation = qgetenv("DEFAULT_ROTATION").toInt();
    }
    return rotation;
}

}    // namespace sys

