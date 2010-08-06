
#include "onyx/wireless/ap_conf_dialog.h"

using namespace ui;

int main(int argc, char * argv[])
{
    QApplication app(argc, argv);

    WifiProfile profile;
    profile.setWep(true);
    ApConfigDialog dialog(0, profile);
    dialog.popup();

    return 0;
}
