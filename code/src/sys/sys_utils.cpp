// -*- mode: c++; c-basic-offset: 4; -*-

#include <stdlib.h>
#ifdef _WINDOWS
#include <windows.h>
#else
#include <sys/sysinfo.h>
#include <sys/vfs.h>
#endif

#include "onyx/sys/sys_utils.h"

namespace sys
{

int runScript(const QString &command, const QStringList & parameters)
{
    QProcess script;
    script.setEnvironment(QProcess::systemEnvironment());
    script.start(command, parameters);
    if (!script.waitForStarted())
    {
        qDebug("Could not start %s", qPrintable(command));
        return -1;
    }

    while (script.state() == QProcess::Running)
    {
        QCoreApplication::processEvents();
    }
    return script.exitCode();
}

int runScriptBlock(const QString &command,
                   const QStringList & parameters,
                   const int timeout)
{
    QProcess script;
    script.setEnvironment(QProcess::systemEnvironment());
    script.start(command, parameters);
    if (!script.waitForStarted())
    {
        qDebug("Could not start %s", qPrintable(command));
        return -1;
    }

    if (!script.waitForFinished(timeout))
    {
        qDebug("Failed");
    }
    return script.exitCode();
}

unsigned long long diskSpace(const QString & mount_ponit)
{
#ifdef _WINDOWS
    return 0;
#else
    struct statfs stats;
    statfs (mount_ponit.toLocal8Bit(), &stats);
    return (static_cast<unsigned long long>(stats.f_blocks) *
            static_cast<unsigned long long>(stats.f_bsize));
#endif
}

unsigned long long freeSpace(const QString & mount_ponit)
{
#ifdef _WINDOWS
    return 0;
#else
    struct statfs stats;
    statfs (mount_ponit.toLocal8Bit(), &stats);
    return (static_cast<unsigned long long>(stats.f_bavail) *
            static_cast<unsigned long long>(stats.f_bsize));
#endif
}

unsigned long systemTotalMemory()
{
#ifdef _WINDOWS
    MEMORYSTATUS info;
    GlobalMemoryStatus(&info);
    return info.dwTotalPhys;
#else
    struct sysinfo info;
    sysinfo(&info);
    return info.totalram * info.mem_unit;
#endif
}

// On Linux, we can also use  cat /proc/<pid>/status to get process memory
// usage.
unsigned long systemFreeMemory()
{
#ifdef _WINDOWS
    MEMORYSTATUS info;
    GlobalMemoryStatus(&info);
    return info.dwAvailPhys;
#else
    struct sysinfo info;
    sysinfo(&info);
    return info.freeram * info.mem_unit;
#endif
}

unsigned long safeMemoryLimit()
{
#ifdef _WINDOWS
    return systemFreeMemory() - (60 << 20);
#else
    return 15 * 1024 * 1024;
#endif
}

bool needReleaseMemory()
{
    return systemFreeMemory() <= safeMemoryLimit();
}

}
