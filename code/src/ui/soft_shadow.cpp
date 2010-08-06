
#include <QtGui/QtGui>
#include "onyx/ui/soft_shadow.h"

namespace ui
{

const int Shadows::PIXELS = 10;

OnyxShadowWidget::OnyxShadowWidget(QWidget *parent, Qt::Orientation o)
    : QDialog(parent, Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint)
    , orient_(o)
{
    setAutoFillBackground(true);
    setBackgroundRole(QPalette::Dark);
    setWindowOpacity(0.6);
}

OnyxShadowWidget::~OnyxShadowWidget()
{
}

void OnyxShadowWidget::resizeEvent(QResizeEvent *e)
{
    QPainterPath path;
    if (orient_ == Qt::Horizontal)
    {
        path = getHorizontalPath();
    }
    else
    {
        path = getVerticalPath();
    }
    QRegion maskedRegion(path.toFillPolygon().toPolygon());
    setMask(maskedRegion);
}

QPainterPath OnyxShadowWidget::getHorizontalPath()
{
    int height  = rect().height() >> 1;
    int x_start = rect().left();
    int x_end   = rect().right();
    int y       = rect().top() + (rect().height() >> 1);

    int radius = Shadows::PIXELS;
    int diameter = (Shadows::PIXELS << 1);

    QPainterPath path;
    path.moveTo(x_end, y);
    path.lineTo(x_start, y);
    path.lineTo(x_start, y + (height - radius));
    path.arcTo(x_start, y + (height - diameter), diameter, diameter, 180.0, 90.0);
    path.lineTo((x_end - radius), y + height);
    path.arcTo((x_end - diameter), y + (height - diameter), diameter, diameter, 270.0, 90.0);
    path.lineTo(x_end, y);
    path.closeSubpath();

    return path;
}

QPainterPath OnyxShadowWidget::getVerticalPath()
{
    int height  = rect().height();
    int x_start = rect().left() + (rect().width() >> 1);
    int x_end   = rect().right();
    int y       = rect().top();

    int radius = Shadows::PIXELS;
    int diameter = (Shadows::PIXELS << 1);

    QPainterPath path;
    path.moveTo(x_end, y + radius);
    path.arcTo((x_end - diameter), y, diameter, diameter, 0.0, 90.0);
    path.lineTo(x_start, y);
    path.lineTo(x_start, y + (height - radius));
    path.arcTo(x_start - (radius << 1), y + (height - diameter), diameter, diameter, 0, -90.0);
    path.lineTo(x_end, y + height);
    path.lineTo(x_end, y);
    path.closeSubpath();

    return path;
}


Shadows::Shadows(QWidget *parent)
: hor_shadow_(parent, Qt::Horizontal)
, ver_shadow_(parent, Qt::Vertical)
{
    hor_shadow_.setFixedHeight(PIXELS << 1);
    ver_shadow_.setFixedWidth(PIXELS << 1);
}

Shadows::~Shadows()
{
    hor_shadow_.close();
    ver_shadow_.close();
}

void Shadows::show(bool show)
{
    if (show)
    {
        hor_shadow_.show();
        ver_shadow_.show();
    }
    else
    {
        hor_shadow_.hide();
        ver_shadow_.hide();
    }
}

void Shadows::onWidgetMoved(QWidget *sibling)
{
    hor_shadow_.move(sibling->frameGeometry().left() + PIXELS - 1, sibling->frameGeometry().bottom()- PIXELS);
    ver_shadow_.move(sibling->frameGeometry().right() - PIXELS, sibling->frameGeometry().top() + PIXELS - 1);
}

void Shadows::onWidgetResized(QWidget *sibling)
{
    hor_shadow_.setFixedWidth(sibling->width());
    ver_shadow_.setFixedHeight(sibling->height() - PIXELS);
    onWidgetMoved(sibling);
}

}

