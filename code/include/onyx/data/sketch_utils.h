#ifndef SKETCH_UTILS_H_
#define SKETCH_UTILS_H_

#include "onyx/base/base.h"
#include "onyx/base/down_cast.h"
#include "onyx/ui/ui.h"

#include "onyx/screen/screen_proxy.h"

#include "onyx/data/sketch_point.h"

using namespace ui;
namespace sketch
{

typedef QString PageKey;

enum SketchStatus
{
    SKETCH_BEGIN = 0,
    SKETCH_MOVE,
    SKETCH_END,
    SKETCH_READY
};

enum EraseSize
{
    ERASE_SIZE_INVALID = INVALID_SKETCH_SHAPE,
    ERASE_SIZE_0 = SKETCH_SHAPE_0,
    ERASE_SIZE_1 = SKETCH_SHAPE_1,
    ERASE_SIZE_2 = SKETCH_SHAPE_2,
    ERASE_SIZE_3 = SKETCH_SHAPE_3,
    ERASE_SIZE_4 = SKETCH_SHAPE_4,
    ERASE_SIZE_UNKNOWN
};

struct SketchContext
{
    SketchColor   color_;         /// color of the sketch
    SketchShape   shape_;         /// shape of the sketch
    ZoomFactor    zoom_;          /// zoom factor
    int           draw_layer_;    /// the drawing layer

    SketchContext() : color_(SKETCH_COLOR_BLACK),
                      shape_(SKETCH_SHAPE_1),
                      zoom_(1.0),
                      draw_layer_(0) {}
};

struct HitTestContext
{
    ZoomFactor    zoom_;          /// zoom factor
    EraseSize     size_;          /// size of eraser
    SketchPoint   last_point_;    /// last point

    HitTestContext() : zoom_(1.0),
                       size_(ERASE_SIZE_4) {}
};

typedef struct HitTestContext EraseContext;


struct StrokeLine
{
    QPoint p1;
    QPoint p2;

    StrokeLine() {}
    StrokeLine(const QPoint &pos1, const QPoint &pos2) : p1(pos1), p2(pos2) {}
    ~StrokeLine() {}
};
typedef QVector<StrokeLine> StrokeLines;
typedef StrokeLines::iterator StrokeLineIter;

/// Sketch position contains the position infomation of a point
/// in real document
struct SketchPosition
{
    QPoint sketch_pos;    /// sketch position in a page
    QPoint widget_pos;    /// sketch position in the widget
    QPoint global_pos;    /// sketch position in the screen
};

int getPenColor(const SketchColor c);
int getPointSize(const SketchShape s, const ZoomFactor z);
void transformCoordinate(const QRect & page_area,
                         const QPoint & stroke_point,
                         const RotateDegree orient,
                         QPoint & result);
void transformCoordinate(const QRect & page_area,
                         const QPoint & stroke_point,
                         const RotateDegree orient,
                         QPoint & result,
                         const ZoomFactor zoom);
};

#endif
