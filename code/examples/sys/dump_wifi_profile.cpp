

#include "onyx/sys/sys_conf.h"
#include "onyx/base/base.h"

using namespace sys;

int main(int argc, char *argv[])
{
    SystemConfig conf;
    WifiProfiles all;
    conf.loadWifiProfiles(all);
    foreach(WifiProfile profile, all)
    {
        profile.debugDump();
    }
    return 0;
}
