#include <QtGui/QtGui>
#include "onyx/screen/screen_proxy.h"
#include "onyx/ui/text_edit.h"
#include "onyx/sys/sys.h"

namespace ui
{

const QString TEXT_EDIT_STYLE = "       \
QTextEdit                               \
{                                       \
    border: 1px solid dark;             \
    background: white;                  \
    selection-background-color: black;  \
    selection-color: white;             \
    font: 24px bold;                    \
    color: black;                       \
    border-width: 1px;                  \
    border-style: solid;                \
    border-radius: 3;                   \
    padding: 0px;                       \
    min-height: 32px;                   \
 }";


OnyxTextEdit::OnyxTextEdit(const QString & text, QWidget *parent)
: QTextEdit(text, parent)
{
    setStyleSheet(TEXT_EDIT_STYLE);
    setLineWrapMode(QTextEdit::WidgetWidth);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFrameShadow(QFrame::Plain);
    setFrameShape(QFrame::Panel);
    QApplication::setCursorFlashTime(0);
}

OnyxTextEdit::~OnyxTextEdit()
{
}

void OnyxTextEdit::keyPressEvent(QKeyEvent * ke)
{
    if (ke->key() == Qt::Key_F21 ||
        ke->key() == Qt::Key_F22 ||
        ke->key() == Qt::Key_F23 ||
        ke->key() == Qt::Key_F24)
    {
        ke->accept();
        return;
    }

    QTextEdit::keyPressEvent(ke);
    ke->accept();
}

void OnyxTextEdit::keyReleaseEvent(QKeyEvent * ke)
{
    if (ke->key() == Qt::Key_F21 ||
        ke->key() == Qt::Key_F22 ||
        ke->key() == Qt::Key_F23 ||
        ke->key() == Qt::Key_F24)
    {
        ke->accept();
        return;
    }

    if (ke->key() == Qt::Key_Escape ||
        ke->key() == ui::Device_Menu_Key)
    {
        ke->ignore();
        return;
    }

    QTextEdit::keyReleaseEvent(ke);
    ke->accept();
}

void OnyxTextEdit::dropEvent(QDropEvent * de)
{
    qDebug("Drop, GU update");
    onyx::screen::instance().enableUpdate(true);
    onyx::screen::instance().updateWidget(this, onyx::screen::ScreenProxy::GU, false);
    QTextEdit::dropEvent(de);
}

void OnyxTextEdit::mousePressEvent(QMouseEvent *me)
{
    pressed_point_ = me->pos();
    onyx::screen::instance().enableUpdate(false);
    QTextEdit::mousePressEvent(me);
    me->accept();
}

void OnyxTextEdit::mouseReleaseEvent(QMouseEvent *me)
{
    QTextEdit::mouseReleaseEvent(me);
    me->accept();
    while (QApplication::hasPendingEvents())
    {
        QApplication::processEvents();
    }
    onyx::screen::instance().enableUpdate(true);

    int direction = sys::SystemConfig::direction(pressed_point_, me->pos());
    if (direction == 0)
    {
        onyx::screen::instance().updateWidget(this, onyx::screen::ScreenProxy::DW, false);
    }
    else
    {
        onyx::screen::instance().updateWidget(this, onyx::screen::ScreenProxy::GU, false);
    }
}

void OnyxTextEdit::mouseDoubleClickEvent(QMouseEvent * event)
{
    event->accept();
}

}
