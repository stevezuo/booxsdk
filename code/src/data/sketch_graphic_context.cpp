#include <algorithm>
#include <math.h>

#include "onyx/data/sketch_graphic_context.h"

namespace sketch
{

using namespace std;

int getPenColor(const SketchColor c)
{
    int pen_color = 0xFF;
    switch (c)
    {
    case SKETCH_COLOR_WHITE:
        break;
    case SKETCH_COLOR_LIGHT_GRAY:
        pen_color = 0x20;
        break;
    case SKETCH_COLOR_DARK_GRAY:
        pen_color = 0x10;
        break;
    case SKETCH_COLOR_BLACK:
        pen_color = 0x00;
        break;
    default:
        break;
    }

    return pen_color;
}

int getPointSize(const SketchShape s, const ZoomFactor z)
{
    int size = 1;
    switch (s)
    {
    case SKETCH_SHAPE_0:
        size = 1;
        break;
    case SKETCH_SHAPE_1:
        size = 2;
        break;
    case SKETCH_SHAPE_2:
        size = 4;
        break;
    case SKETCH_SHAPE_3:
        size = 6;
        break;
    case SKETCH_SHAPE_4:
        size = 8;
        break;
    default:
        break;
    }

    size = static_cast<int>(static_cast<ZoomFactor>(size) * z);
    if (size > 1)
    {
        size = ((size >> 1) << 1);
    }
    else if (size < 1)
    {
        size = 1;
    }
    return size;
}

GraphicContext::GraphicContext(QWidget *w,
                               RotateDegree co,
                               RotateDegree wo)
    : drawing_area_(w)
    , content_orient_(co)
    , widget_orient_(wo)
    , color_(SKETCH_COLOR_BLACK)
    , shape_(SKETCH_SHAPE_3)
    , img_(0)
{
}

GraphicContext::~GraphicContext()
{
    if (img_ != 0)
    {
        delete [] img_;
    }
}

void GraphicContext::fastDrawLine(const QPoint & p1,
                                  const QPoint & p2,
                                  const SketchContext & ctx)
{
    int pen_color  = getPenColor(ctx.color_);
    int point_size = getPointSize(ctx.shape_, ctx.zoom_);

    // Fast draw line
    onyx::screen::instance().drawLine(p1.x(), p1.y(), p2.x(), p2.y(), pen_color, point_size);
#ifndef ENABLE_EINK_SCREEN
    QPainter painter(drawing_area_);
    drawLine(p1, p2, ctx, painter);
#endif
}

void GraphicContext::fastDrawLines(QVector<QPoint> & points, const SketchContext & ctx)
{
    int pen_color  = getPenColor(ctx.color_);
    int point_size = getPointSize(ctx.shape_, ctx.zoom_);

    // Fast draw line
    onyx::screen::instance().drawLines(points.data(), points.size(), pen_color, point_size);

#ifndef ENABLE_EINK_SCREEN
    QPainter painter(drawing_area_);
    QPoint p1, p2;
    for (int i = 0; i < points.size() - 1; ++i)
    {
        p1 = points[i];
        p2 = points[i + 1];
        drawLine(p1, p2, ctx, painter);
    }

#endif
}

void GraphicContext::drawLine(const QPoint & p1,
                              const QPoint & p2,
                              const SketchContext & ctx,
                              QPainter & painter)
{
    // set color and size of brush
    int brush_color = getPenColor(ctx.color_);
    int point_size = getPointSize(ctx.shape_, ctx.zoom_);
    QBrush brush(QColor(brush_color, brush_color, brush_color));

    // Use bresham algorithm to draw the line
    int rad = point_size / 2;
    int px1 = p1.x() - rad;
    int py1 = p1.y() - rad;
    int px2 = p2.x() - rad;
    int py2 = p2.y() - rad;

    // draw the end point
    //painter.drawRect(px2, py2, point_size, point_size);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(px2, py2, point_size, point_size, brush);

    bool is_steep = abs(py2 - py1) > abs(px2 - px1);
    if (is_steep)
    {
        swap(px1, py1);
        swap(px2, py2);
    }

    // setup line draw
    int deltax   = abs(px2 - px1);
    int deltaerr = abs(py2 - py1);
    int error = 0;
    int x = px1;
    int y = py1;

    // setup step increment
    int xstep = (px1 < px2) ? 1 : -1;
    int ystep = (py1 < py2) ? 1 : -1;

    if (is_steep)
    {
        // draw swapped line
        for (int numpix = 0; numpix < deltax; numpix++)
        {
            x += xstep;
            error += deltaerr;

            if (2 * error > deltax)
            {
                y += ystep;
                error -= deltax;
            }

            // draw the interposed point
            //painter.drawRect(y, x, point_size, point_size);
            painter.fillRect(y, x, point_size, point_size, brush);
        }
    }
    else
    {
        // draw normal line
        for (int numpix = 0; numpix < deltax; numpix++)
        {
            x += xstep;
            error += deltaerr;

            if (2 * error > deltax)
            {
                y += ystep;
                error -= deltax;
            }

            // draw the interposed point
            //painter.drawRect(x, y, point_size, point_size);
            painter.fillRect(x, y, point_size, point_size, brush);
        }
    }

#ifndef ENABLE_EINK_SCREEN
    assert(drawing_area_ != 0);
    drawing_area_->update(QRect(p1, p2).normalized()
                                       .adjusted(-rad, -rad, +rad, +rad));
#endif
}

void GraphicContext::updateWidget(const QRect & area)
{
    assert(drawing_area_ != 0);
    //drawing_area_->update(area);
    // TODO. make a wise algorithm to update the qt buffer.
    //drawing_area_->update();
}

void GraphicContext::updateImage(const SketchContext ctx)
{
    if (ctx.color_ == color_ && ctx.shape_ == shape_ && img_ != 0)
    {
        return;
    }

    color_ = ctx.color_;
    shape_ = ctx.shape_;

    // delete the previous image
    delete [] img_;

    int pen_color = getPenColor(color_);
    int size = getPointSize(shape_, ctx.zoom_);
    int len = size * size;
    img_ = new unsigned char[len];
    for (int i = 0; i < len; ++i)
    {
        img_[i] = pen_color;
    }
}

}
