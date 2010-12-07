
#include "onyx/sys/platform.h"
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
            p = "IMX31L";
        }
    }
    return p;
}

bool is166e()
{
    return (platform().compare("166e", Qt::CaseInsensitive) == 0);
}

bool isIMX31L()
{
    return (platform().compare("IMX31L", Qt::CaseInsensitive) == 0);
}

QString soundModule()
{
    static QString mod = qgetenv("SOUND_MODULE");
    if (mod.isEmpty())
    {
        if (isIMX31L())
        {
            mod = "snd-soc-imx-3stack-wm8711";
        }
        else if (is166e())
        {
            mod = "snd-soc-booxe";
        }
        else
        {
            qDebug("No sound module found, check profile.");
        }
    }
    return mod;
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

