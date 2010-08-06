// Author: John

/// This is the main header file for the UI package. The ui namespace
/// encapsulate the classes we use in our user-facing applications.

#ifndef ONYX_UI_TEXT_LAYOUT_H__
#define ONYX_UI_TEXT_LAYOUT_H__

#include <Qt/QtCore>
#include <Qt/QtGui>


namespace ui
{

int ellipsisText(const QString &org,
                 const QFontMetrics &fm,
                 const int width,
                 const Qt::TextElideMode mode,
                 QString & output);


int calculateSingleLineLayout(QTextLayout & layout,
                              const QFont & font,
                              const QString & text,
                              const Qt::Alignment align,
                              const QRect & rect,
                              const Qt::TextElideMode ellipsis = Qt::ElideRight);


int calculateMultiLinesLayout(QTextLayout & layout,
                              const QFont & font,
                              const QString & text,
                              const Qt::Alignment align,
                              const QRect & rect);

int calculateMultiLinesLayout(const QFont & font,
                              const QString & text,
                              int width);

int calculateMultiLinesLayout(QTextLayout & layout,
                              const QFont & font,
                              const QString & text,
                              const Qt::Alignment align,
                              const QRect & rect,
                              int lines);


void drawSingleLineText(QPainter &painter,
                        const QFont & font,
                        const QString & text,
                        const Qt::Alignment align,
                        const QRect & rect);

}  // namespace ui

#endif  // ONYX_UI_TEXT_LAYOUT_H__
