#ifndef SKETCH_GRAPHIC_CONTEXT_H_
#define SKETCH_GRAPHIC_CONTEXT_H_

#include "onyx/data/sketch_utils.h"

namespace sketch
{
class GraphicContext
{
public:
    GraphicContext(QWidget *w = 0,
                   RotateDegree co = ROTATE_0_DEGREE,
                   RotateDegree wo = ROTATE_0_DEGREE);
    ~GraphicContext();

    void setDrawingArea(QWidget *w) { drawing_area_ = w; }
    const RotateDegree contentOrient() const { return content_orient_; }
    void setContentOrient(const RotateDegree r) { content_orient_ = r;}
    const RotateDegree widgetOrient() const { return widget_orient_; }
    void setWidgetOrient(const RotateDegree r) { widget_orient_ = r; }
    QRect getDrawingAreaRect() { return drawing_area_ != 0 ? drawing_area_->rect() : QRect(); }
    QRect getScreenRect() { return QApplication::desktop()->screenGeometry(); }

    void drawLine(const QPoint & p1,
                  const QPoint & p2,
                  const SketchContext & ctx,
                  QPainter & painter);
    void fastDrawLine(const QPoint & p1,
                      const QPoint & p2,
                      const SketchContext & ctx);
    void fastDrawLines(QVector<QPoint> & points, const SketchContext & ctx);
    void updateWidget(const QRect & area);

private:
    void updateImage(const SketchContext ctx);

private:
    QWidget         *drawing_area_;
    RotateDegree    content_orient_;
    RotateDegree    widget_orient_;
    SketchColor     color_;
    SketchShape     shape_;
    unsigned char * img_;
};

};

#endif
