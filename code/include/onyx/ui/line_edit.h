#ifndef ONYX_LINE_EDIT_H_
#define ONYX_LINE_EDIT_H_

#include <QtGui/QtGui>

namespace ui
{

/// Line edit for eink device. Remove unnecessary updates.
class OnyxLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    OnyxLineEdit(QWidget *parent);
    OnyxLineEdit(const QString & text, QWidget *parent);
    ~OnyxLineEdit();

protected:
    void focusInEvent(QFocusEvent *e);

Q_SIGNALS:
    void getFocus(OnyxLineEdit *object);

protected:
    void keyReleaseEvent(QKeyEvent *ke);
    void keyPressEvent(QKeyEvent * ke);
    void mouseReleaseEvent(QMouseEvent * event);
};

};

#endif  // ONYX_LINE_EDIT_H_
