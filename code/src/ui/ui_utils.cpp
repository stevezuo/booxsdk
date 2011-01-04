
#include "onyx/ui/ui_utils.h"
#include "onyx/sys/sys_status.h"
#include "onyx/sys/platform.h"

namespace ui
{

QRect screenGeometry()
{
    QRect rc = QApplication::desktop()->geometry();
    int def_rotation = sys::defaultRotation();
    int r1 = (def_rotation + 90) % 360;
    int r2 = (def_rotation + 270) % 360;
    if (sys::SysStatus::instance().screenTransformation() == r1 ||
        sys::SysStatus::instance().screenTransformation() == r2)
    {
        int w = rc.width();
        rc.setWidth(rc.height());
        rc.setHeight(w);
    }
    return rc;
}

bool dockWidget(QWidget *target, QWidget * container, Qt::Alignment align)
{
    return true;
}

}


