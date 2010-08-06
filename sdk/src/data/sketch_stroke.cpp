#include "onyx/data/sketch_stroke.h"

namespace sketch
{
static const ZoomFactor ZOOM_ERROR = 0.001f;

SketchStroke::SketchStroke(void)
: color_(SKETCH_COLOR_BLACK)
, shape_(SKETCH_SHAPE_3)
, zoom_(1.0)
, draw_layer_(0)
{
}

SketchStroke::SketchStroke(SketchContext & ctx)
: color_(ctx.color_)
, shape_(ctx.shape_)
, zoom_(ctx.zoom_)
, draw_layer_(ctx.draw_layer_)
{
}

SketchStroke::~SketchStroke(void)
{
}

/// Add a point into stroke
void SketchStroke::addPoint(const SketchPoint & p)
{
    // append point in points array
    points_.append(p);

    if (area_.isNull())
    {
        area_.setTopLeft(p);
    }
    else
    {
        // expand the area of stroke
        if (area_.left() > p.x())
        {
            area_.setLeft(p.x());
        }

        if (area_.right() < p.x())
        {
            area_.setRight(p.x());
        }

        if (area_.top() > p.y())
        {
            area_.setTop(p.y());
        }

        if (area_.bottom() < p.y())
        {
            area_.setBottom(p.y());
        }
    }
}

void SketchStroke::paint(const SketchContext & sketch_ctx,
                         const QRect & page_area,
                         GraphicContext & gc,
                         QPainter & painter)
{
    // get points of current stroke
    int count = points_.size();
    ZoomFactor cur_zoom = sketch_ctx.zoom_;
    ZoomFactor stroke_zoom = zoom();
    ZoomFactor ratio = 1.0f;
    if (fabs(cur_zoom - stroke_zoom) > ZOOM_ERROR)
    {
        ratio = cur_zoom / stroke_zoom;
    }

    SketchContext ctx = sketch_ctx;
    ctx.color_ = color();
    ctx.shape_ = shape();
    ctx.draw_layer_ = layer();
    ctx.zoom_ = ratio;
    if (count > 0)
    {
        QPoint pos1, pos2;

        // get the first point
        int idx = 0;
        transformCoordinate(page_area,
                            points_[idx],
                            gc.contentOrient(),
                            pos1,
                            ratio);
        idx++;

        while (idx < count)
        {
            transformCoordinate(page_area,
                                points_[idx],
                                gc.contentOrient(),
                                pos2,
                                ratio);
            gc.drawLine(pos1, pos2, ctx, painter);
            idx++;
            pos1 = pos2;
        }

        if (count <= 1)
        {
            gc.drawLine(pos1, pos1, ctx, painter);
        }
    }
}

/// Hit test whether a point is in area of stroke
bool SketchStroke::hitTest(const QPoint & p, const HitTestContext & ctx)
{
    // 1. quick check the bounding area of stroke and hit point
    float ratio = 1.0f;
    if (fabs(zoom_ - ctx.zoom_) >= ZOOM_ERROR)
    {
        ratio = zoom_ / ctx.zoom_;
    }

    int size = getPointSize(static_cast<SketchShape>(ctx.size_), ratio);
    int len = (size << 1);
    QRect hit_rect(static_cast<int>(p.x() * ratio) - size,
                   static_cast<int>(p.y() * ratio) - size,
                   len,
                   len);
    if (!hit_rect.intersects(area_))
    {
        return false;
    }

    // 2. check every point of the stroke
    for (int i = 0; i < points_.size(); ++i)
    {
        if (hit_rect.contains(points_[i]))
        {
            return true;
        }
    }

    return false;
}

/// Hit test whether a line is intersected of a stroke
bool SketchStroke::hitTest(const QLine & line, const HitTestContext & ctx)
{
    // 1. quick check the line
    // expand the line
    QPoint p1 = line.p1();
    QPoint p2 = line.p2();
    float ratio = 1.0;
    if (fabs(zoom_ - ctx.zoom_) >= ZOOM_ERROR)
    {
        ratio = zoom_ / ctx.zoom_;
        p1.setX(static_cast<int>(p1.x() * ratio));
        p1.setY(static_cast<int>(p1.y() * ratio));
        p2.setX(static_cast<int>(p2.x() * ratio));
        p2.setY(static_cast<int>(p2.y() * ratio));
    }

    int size = getPointSize(static_cast<SketchShape>(ctx.size_), ratio);
    QRect hit_rect;
    hit_rect.setLeft(qMin(p1.x(), p2.x()) - size);
    hit_rect.setTop(qMin(p1.y(), p2.y()) - size);
    hit_rect.setRight(qMax(p1.x(), p2.x()) + size);
    hit_rect.setBottom(qMax(p1.y(), p2.y()) + size);

    if (!hit_rect.intersects(area_))
    {
        return false;
    }

    // check every line in stroke
    QLineF hit_line(p1, p2);
    if (points_.size() > 1)
    {
        for (int idx = 0; idx < (points_.size() - 1); ++idx)
        {
            QLineF stroke_line(points_[idx], points_[idx + 1]);
            QPointF intersect_point;
            if (stroke_line.intersect(hit_line, &intersect_point) ==
                QLineF::BoundedIntersection)
            {
                return true;
            }
        }
    }
    else
    {
        QLineF stroke_line(points_[0], points_[0]);
        QPointF intersect_point;
        if (stroke_line.intersect(hit_line, &intersect_point) ==
            QLineF::BoundedIntersection)
        {
            return true;
        }
    }

    return false;
}

int SketchStroke::length()
{
    int len = getLengthOfAttributes();

    int points_num = points_.size();
    len += points_num * sizeof(SketchPoint);

    return len;
}

int SketchStroke::getLengthOfAttributes()
{
    // the length of points is also one attribute
    return (sizeof(color_) + sizeof(shape_) + sizeof(zoom_) +
        sizeof(draw_layer_) + sizeof(int));
}

QDataStream& operator<<(QDataStream & out, const SketchStroke & stroke)
{
    if (stroke.dumpAttributes(out))
    {
        stroke.dumpPoints(out);
    }

    return out;
}

bool SketchStroke::dumpAttributes(QDataStream & out) const
{
    out << static_cast<qint16>(color_);
    out << static_cast<qint16>(shape_);
    out << zoom_;
    out << static_cast<qint16>(draw_layer_);
    out << static_cast<qint16>(points_.size());
    out << area_;
    return true;
}

bool SketchStroke::dumpPoints(QDataStream & out) const
{
    out << points_;
    return true;
}

QDataStream& operator>>(QDataStream & in, SketchStroke & stroke)
{
    int num_points = 0;
    if (stroke.loadAttributes(in, num_points))
    {
        stroke.loadPoints(in, num_points);
    }

    return in;
}

bool SketchStroke::loadAttributes(QDataStream & in, int & num_points)
{
    qint16 c = 0;
    in >> c;
    color_ = static_cast<SketchColor>(c);

    qint16 s = 0;
    in >> s;
    shape_ = static_cast<SketchShape>(s);

    in >> zoom_;

    qint16 l;
    in >> l;
    draw_layer_ = l;

    qint16 num;
    in >> num;
    num_points = num;

    in >> area_;
    return true;
}

bool SketchStroke::loadPoints(QDataStream & in, const int num_points)
{
    points_.clear();
    in >> points_;
    return true;
}

}
