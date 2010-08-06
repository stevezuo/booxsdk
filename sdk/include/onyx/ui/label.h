#ifndef ONYX_LABEL_H_
#define ONYX_LABEL_H_

#include <QtGui/QtGui>

namespace ui
{

/// Label for eink device. Use the stylesheet for eink device.
class OnyxLabel : public QLabel
{
public:
    OnyxLabel(QWidget * parent = 0);
    OnyxLabel(const QString & text, QWidget *parent = 0);
    ~OnyxLabel();

public:
    void useTitleBarStyle();
    void useMessageStyle(int width = 0);
    void squeezeText(Qt::TextElideMode mode = Qt::ElideMiddle);
};

};

#endif  // ONYX_LABEL_H_
