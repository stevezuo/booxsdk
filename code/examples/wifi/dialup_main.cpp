
#include "onyx/wireless/dialup_dialog.h"
#include "onyx/sys/sys_status.h"
#include <QtGui/QtGui>

using namespace sys;
using namespace ui;

int main(int argc, char * argv[])
{
    QApplication app(argc, argv);

    Q_INIT_RESOURCE(onyx_ui_images);
    Q_INIT_RESOURCE(wifi_images);
    DialUpDialog dialog(0, SysStatus::instance());
    dialog.popup();
    app.setQuitOnLastWindowClosed(true);
    return app.exec();
}

