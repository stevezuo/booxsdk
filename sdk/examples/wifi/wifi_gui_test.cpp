#include <QtGui/QtGui>

#include "onyx/wireless/wifi_dialog.h"
#include "onyx/sys/sys_status.h"

using namespace sys;
using namespace ui;

int main(int argc, char * argv[])
{
    QApplication app(argc, argv);

    Q_INIT_RESOURCE(onyx_ui_images);
    Q_INIT_RESOURCE(wifi_images);
    WifiDialog dialog(0, SysStatus::instance());
    dialog.popup();
    return app.exec();
}

