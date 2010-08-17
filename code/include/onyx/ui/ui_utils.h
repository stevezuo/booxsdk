#ifndef ONYX_UI_UTILS_H_
#define ONYX_UI_UTILS_H_

#include <QtGui/QtGui>

namespace ui
{

// Return the screen geometry when transform.
QRect screenGeometry();

bool dockWidget(QWidget *target, QWidget * container, Qt::Alignment align);

};

#endif  // ONYX_UI_UTILS_H_

