#ifndef SKETCH_POINT_H_
#define SKETCH_POINT_H_

#include "onyx/base/base.h"
#include "onyx/base/down_cast.h"
#include "onyx/ui/ui_global.h"

namespace sketch
{

class SketchPoint : public QPoint
{
public:
    SketchPoint(void);
    SketchPoint(int x, int y, int p = 0);
    ~SketchPoint(void);

    // reset the point to be invalid
    void reset();
    bool isValid();

    // set pressure
    int pressure() const { return pressure_; }
    void setPressure(int p) { pressure_ = p; }

    SketchPoint & operator=(const QPoint & p);

private:
    int pressure_;    // pressure of a sketch point
};

QDataStream & operator<<(QDataStream & out, const SketchPoint & point);
QDataStream & operator>>(QDataStream & in, SketchPoint & point);

typedef QVector<SketchPoint>  Points;
typedef Points::iterator      PointsIter;

};

#endif
