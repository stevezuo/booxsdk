#ifndef ONYX_TEXT_EDIT_H_
#define ONYX_TEXT_EDIT_H_

#include "onyx/base/base.h"
#include <QtGui/QtGui>

namespace ui
{

/// Text edit for eink device.
class OnyxTextEdit : public QTextEdit
{
public:
    OnyxTextEdit(const QString & text, QWidget *parent);
    ~OnyxTextEdit();

protected:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void mouseDoubleClickEvent ( QMouseEvent * event );

    virtual void keyPressEvent(QKeyEvent * ke);
    virtual void keyReleaseEvent(QKeyEvent * ke);

    virtual void dropEvent(QDropEvent * de);
private:
    scoped_ptr<QKeyEvent> key_event_;
    QPoint pressed_point_;
};

};

#endif  // ONYX_TEXT_EDIT_H_
