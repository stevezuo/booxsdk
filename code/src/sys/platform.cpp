
#include "onyx/sys/platform.h"
#include <QtCore/qglobal.h>

namespace sys
{
static const QString IM31L = "imx31L";
static const QString MARVELL = "166e";

QString platform()
{
    static QString p;
    if (p.isEmpty())
    {
        p = qgetenv("PLATFORM");
        if (p.isEmpty())
        {
            p = IM31L;
        }
    }
    return p;
}

bool isIMX31L()
{
    return platform() == IM31L;
}

bool is166E()
{
    return platform() == MARVELL;
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
