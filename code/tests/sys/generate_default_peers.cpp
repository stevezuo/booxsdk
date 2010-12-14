#include <QtGui/QtGui>
#include <cassert>

#include "onyx/base/qt_support.h"
#include "onyx/sys/sys_conf.h"


using namespace sys;


struct APN
{
    QString display_name;
    QString username;
    QString password;
    QString apn;
};

static const APN APNS[] =
{
    {"Mts", "mts", "mts", "mts"},
    {"Megafon", "gdata", "gdata", "megafon"},
    {"Beeline", "beeline", "beeline", "beeline"},
    {"telenor", "telenor", "", "telenor"},
    {"T-Mobile", "", "", "t-mobile"},
    {"Orange", "", "", "orange"},
    {"O2", "", "", "o2"},
    {"movistar.es", "movistar", "movistar", "movistar"},
    {"web.htgprs", "", "", "htgprs"},
    {"China Unicom", "", "", "unicom"},
    {"China Telecom", "", "", "telecom"},
};
static const int APNS_COUNT = sizeof(APNS)/sizeof(APNS[0]);


int main(int argc, char * argv[])
{
    SystemConfig conf;
    DialupProfiles all;
    conf.loadDialupProfiles(all);
    all.clear();

    for(int i = 0; i < APNS_COUNT; ++i)
    {
        DialupProfile a;
        a.setDisplayName(APNS[i].display_name);
        a.setUsername(APNS[i].username);
        a.setPassword(APNS[i].password);
        a.setApn(APNS[i].apn);
        all.push_back(a);
    }
    conf.saveDialupProfiles(all);
}

