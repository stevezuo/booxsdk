#include <QtGui/QtGui>
#include <cassert>

#include "onyx/base/qt_support.h"
#include "onyx/screen/screen_proxy.h"


int main(int argc, char * argv[])
{
    while (1)
    {
        onyx::screen::ScreenProxy::instance().flush(0);
    }
    return 0;
}
