
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

