#ifndef SKETCH_STROKE_H_
#define SKETCH_STROKE_H_

#include "onyx/data/sketch_utils.h"
#include "onyx/data/sketch_point.h"
#include "onyx/data/sketch_graphic_context.h"

using namespace ui;
namespace sketch
{

class SketchStroke
{
public:
    SketchStroke(void);
    SketchStroke(SketchContext & ctx);
    ~SketchStroke(void);

    void setColor(const SketchColor c) { color_ = c; }
    SketchColor color() const { return color_; }

    void setShape(const SketchShape s) { shape_ = s; }
    SketchShape shape() const { return shape_; }

    void setZoom(const ZoomFactor z) { zoom_ = z; }
    ZoomFactor zoom() const { return zoom_; }

    void setLayer(const int l) { draw_layer_ = l; }
    int layer() const { return draw_layer_; }

    void addPoint(const SketchPoint & p);
    Points & points() { return points_; }

    const QRect & area() const { return area_; }

    // hit test
    bool hitTest(const QPoint & p, const HitTestContext & ctx);
    bool hitTest(const QLine & line, const HitTestContext & ctx);

    // io
    int  length();

    // paint
    void paint(const SketchContext & sketch_ctx,
               const QRect & page_area,
               GraphicContext & gc,
               QPainter & painter);

private:
    // io
    int  getLengthOfAttributes();
    bool dumpAttributes(QDataStream & out) const;
    bool dumpPoints(QDataStream & out) const;
    bool loadAttributes(QDataStream & in, int & num_points);
    bool loadPoints(QDataStream & in, const int num_points);

    friend QDataStream& operator<<(QDataStream & out, const SketchStroke & stroke);
    friend QDataStream& operator>>(QDataStream & in, SketchStroke & stroke);

private:
    // attributes that need to be saved
    SketchColor   color_;         /// color of the sketch
    SketchShape   shape_;         /// shape of the sketch
    ZoomFactor    zoom_;          /// zoom factor
    int           draw_layer_;    /// the drawing layer
    QRect         area_;          /// the display area of the stroke
    Points        points_;        /// the array of points
};

QDataStream& operator<<(QDataStream & out, const SketchStroke & stroke);
QDataStream& operator>>(QDataStream & in, SketchStroke & stroke);

typedef shared_ptr<SketchStroke> SketchStrokePtr;
typedef QVector<SketchStrokePtr> Strokes;
typedef Strokes::iterator StrokesIter;

};

#endif
