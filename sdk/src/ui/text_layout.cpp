#include "onyx/ui/text_layout.h"

namespace ui
{

/// Calculate text layout length according to the font metrics.
/// This function returns 0 if the text is too long to display.
/// It returns text displayed size when the whole string can be
/// displayed within specified with.
int ellipsisText(const QString &org,
                 const QFontMetrics &fm,
                 const int width,
                 const Qt::TextElideMode mode,
                 QString & output)
{
    int real_width = fm.width(org);
    if (real_width <= width)
    {
        return real_width;
    }

    output = fm.elidedText(org, mode, width);
    return width;
}


/// Calculate single line layout for given text with specified font.
/// This function automatically add ellipsis text if the naural width
/// is larger than given region.
/// \layout The result layout.
/// \font The font object.
/// \string The text needs to be layouted.
/// \align Alignment.
/// \rect The region.
/// \ellipsis Add ellipse to left or right if necessary.
/// Returns the line height.
int calculateSingleLineLayout(QTextLayout & layout,
                              const QFont & font,
                              const QString & string,
                              const Qt::Alignment align,
                              const QRect & rect,
                              const Qt::TextElideMode ellipsis)
{
    QFontMetrics fm(font);

    // Check if we need to add ellipsis.
    QString result_string = string;
    int width = ellipsisText(string, fm, rect.width(),
                             ellipsis, result_string);

    // Construct the layout.
    layout.clearLayout();
    layout.setCacheEnabled(false);
    layout.setText(result_string);
    layout.setFont(font);
    layout.beginLayout();
    QTextLine line = layout.createLine();
    if (line.isValid())
    {
        line.setLineWidth(rect.width());
    }
    layout.endLayout();

    // Calculate the position.
    int x = rect.left();
    int y = rect.top();
    int h = static_cast<int>(line.height());

    if (align & Qt::AlignHCenter)
    {
        x = ((rect.width() - width) >> 1) + x;
    }
    if (align & Qt::AlignRight)
    {
        x = (rect.width() - width);
    }
    if (align & Qt::AlignVCenter)
    {
        y = ((rect.height() - h) >> 1) + y;
    }
    if (align & Qt::AlignBottom)
    {
        y = rect.height() - h;
    }
    layout.setPosition(QPoint(x, y));
    return static_cast<int>(line.height());
}

int calculateMultiLinesLayout(QTextLayout & layout,
                              const QFont & font,
                              const QString & string,
                              const Qt::Alignment align,
                              const QRect & rect)
{
    // Construct the layout.
    layout.setCacheEnabled(false);
    layout.setText(string);
    layout.setFont(font);
    QTextOption option = layout.textOption();
    option.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    layout.setTextOption(option);

    layout.beginLayout();
    QTextLine line = layout.createLine();
    int total = 0;
    while (line.isValid())
    {
        line.setLineWidth(rect.width());

        if (align & Qt::AlignLeft)
        {
            line.setPosition(QPoint(0, total));
        }
        else if (align & Qt::AlignHCenter)
        {
            int x = static_cast<int>((rect.width() - line.naturalTextWidth()) / 2);
            line.setPosition(QPoint(x, total));
        }
        else if (align & Qt::AlignRight)
        {
            int x = static_cast<int>(rect.width() - line.naturalTextWidth());
            line.setPosition(QPoint(x, total));
        }
        total += static_cast<int>(line.height());
        if (total + line.height() > rect.height())
        {
            break;
        }
        line = layout.createLine();
    }
    layout.endLayout();
    layout.setPosition(rect.topLeft());
    return total;
}

/// Return height of text layout.
int calculateMultiLinesLayout(const QFont & font,
                              const QString & text,
                              int width)
{
    // Construct the layout.
    QTextLayout layout;
    layout.setCacheEnabled(false);
    layout.setText(text);
    layout.setFont(font);
    QTextOption option = layout.textOption();
    option.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    layout.setTextOption(option);

    layout.beginLayout();
    QTextLine line = layout.createLine();
    int total = 0;
    while (line.isValid())
    {
        line.setLineWidth(width);
        total += static_cast<int>(line.height());
        line = layout.createLine();
    }
    layout.endLayout();
    return total;
}


/// Return the layout is need to update or not.
static bool calculateLayout(QTextLayout & layout,
                            const QFont & font,
                            QString & text,
                            const Qt::Alignment align,
                            const QRect & rect,
                            int lines)
{
    layout.setCacheEnabled(false);
    layout.setText(text);
    layout.setFont(font);
    QTextOption option = layout.textOption();
    option.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    layout.setTextOption(option);

    layout.beginLayout();
    QTextLine line = layout.createLine();
    int count = 1;
    while (line.isValid() && count < lines)
    {
        line.setLineWidth(rect.width());
        line = layout.createLine();
        ++count;
    }

    if (!line.isValid())
    {
        layout.clearLayout();
        return false;
    }

    // Last line.
    QString string = text.mid(line.textStart());
    QString result_string = string;
    QFontMetrics fm(font);
    ellipsisText(string, fm, rect.width(), Qt::ElideRight, result_string);

    if (result_string == string)
    {
        layout.clearLayout();
        return false;
    }

    text = text.left(line.textStart()) + result_string;
    layout.clearLayout();
    return true;
}

/// Calculate text layout with specified line limit.
int calculateMultiLinesLayout(QTextLayout & layout,
                              const QFont & font,
                              const QString & text,
                              const Qt::Alignment align,
                              const QRect & rect,
                              int lines)
{
    // Fallback to the other function.
    if (lines < 0)
    {
        return calculateMultiLinesLayout(layout, font, text, align, rect);
    }

    QFontMetrics fm(font);
    if (fm.height() * 2 > rect.height())
    {
        return calculateSingleLineLayout(layout, font, text, align, rect);
    }

    QString tmp(text);
    calculateLayout(layout, font, tmp, align, rect, lines);

    // Construct the layout.
    layout.setCacheEnabled(false);
    layout.setText(tmp);
    layout.setFont(font);
    QTextOption option = layout.textOption();
    option.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    layout.setTextOption(option);

    layout.beginLayout();
    int count = 1;
    int total = 0;
    QTextLine line = layout.createLine();
    while (line.isValid() && count <= lines)
    {
        line.setLineWidth(rect.width());
        if (align & Qt::AlignLeft)
        {
            line.setPosition(QPoint(0, total));
        }
        else if (align & Qt::AlignHCenter)
        {
            int x = static_cast<int>((rect.width() - line.naturalTextWidth()) / 2);
            line.setPosition(QPoint(x, total));
        }
        else if (align & Qt::AlignRight)
        {
            int x = static_cast<int>(rect.width() - line.naturalTextWidth());
            line.setPosition(QPoint(x, total));
        }
        total += static_cast<int>(line.height());
        line = layout.createLine();
        ++count;
    }

    int y = rect.top();
    if (align & Qt::AlignVCenter)
    {
        y = ((rect.height() - total) >> 1) + y;
    }
    if (align & Qt::AlignBottom)
    {
        y = y + rect.height() - total;
    }

    layout.setPosition(QPoint(rect.left(), y));
    layout.endLayout();
    return total;
}

void drawSingleLineText(QPainter &painter,
                        const QFont & font,
                        const QString & text,
                        const Qt::Alignment align,
                        const QRect & rect)
{

    QTextLayout layout;
    calculateSingleLineLayout(layout, font, text, align, rect);
    layout.draw(&painter, QPointF(0, 0));
}


}


