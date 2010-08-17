
#include "onyx/ui/ui_utils.h"
#include "onyx/sys/sys_status.h"

namespace ui
{

QRect screenGeometry()
{
    QRect rc = QApplication::desktop()->geometry();
    if (sys::SysStatus::instance().screenTransformation() == 90 ||
        sys::SysStatus::instance().screenTransformation() == 270)
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


