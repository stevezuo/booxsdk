
#include <QtGui/QtGui>
#include "onyx/ui/label.h"
#include "onyx/ui/text_layout.h"

const QString LABEL_STYLE = "           \
QLabel                                  \
{                                       \
     padding: 0px;                      \
     background: transparent;           \
     font: 24px ;                       \
     color: black;                      \
 }";

const QString TITLEBAR_STYLE = "        \
QLabel                                  \
{                                       \
     padding: 0px;                      \
     background: transparent;           \
     font: 24px ;                       \
     color: white;                      \
 }";

const QString MESSAGE_STYLE = "         \
QLabel                                  \
{                                       \
    background: white;                  \
    font: 24px ;                        \
    border-width: 1px;                  \
    border-color: white;                \
    border-style: solid;                \
    border-radius: 3;                   \
    color: black;                       \
    padding: 0px;                       \
 }";

namespace ui
{

OnyxLabel::OnyxLabel(QWidget * parent)
: QLabel(parent, 0)
{
    setStyleSheet(LABEL_STYLE);
    setFocusPolicy(Qt::NoFocus);
}

OnyxLabel::OnyxLabel(const QString & text, QWidget *parent)
: QLabel(text, parent)
{
    setStyleSheet(LABEL_STYLE);
    setFocusPolicy(Qt::NoFocus);
}

OnyxLabel::~OnyxLabel()
{
}

void OnyxLabel::useTitleBarStyle()
{
    setStyleSheet(TITLEBAR_STYLE);
}

void OnyxLabel::useMessageStyle(int w)
{
    const int PADDING = 10;
    setContentsMargins(PADDING, PADDING, PADDING, PADDING);
    setStyleSheet(MESSAGE_STYLE);
    setWordWrap(true);

    // Calculate height.
    int h = 0;
    if (w <= 0)
    {
        h = heightForWidth(width() - 2 * PADDING);
    }
    else
    {
        h = heightForWidth(w - 2 * PADDING);
    }
    setFixedSize(w, h + 2 * PADDING);
}

void OnyxLabel::squeezeText(Qt::TextElideMode mode)
{
    QFontMetrics fm = fontMetrics();
    if (fm.width(text()) > contentsRect().width())
    {
        QString elided = fm.elidedText(text(), mode, width());
        setText(elided);
    }
}

}
