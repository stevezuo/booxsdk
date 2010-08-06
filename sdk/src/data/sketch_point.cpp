
#include "onyx/data/sketch_point.h"

namespace sketch
{

SketchPoint::SketchPoint(void)
: QPoint()
, pressure_(0)
{
    reset();
}

SketchPoint::SketchPoint(int x, int y, int p)
: QPoint(x, y)
, pressure_(p)
{
}

SketchPoint::~SketchPoint(void)
{
}

void SketchPoint::reset()
{
    setX(-1);
    setY(-1);
}

bool SketchPoint::isValid()
{
    return (x() >= 0 && y() >= 0);
}

SketchPoint & SketchPoint::operator=(const QPoint & p)
{
    setX(p.x());
    setY(p.y());
    return *this;
}

QDataStream& operator<<(QDataStream & out, const SketchPoint & point)
{
    out << static_cast<qint16>(point.x());
    out << static_cast<qint16>(point.y());
    out << static_cast<qint16>(point.pressure());
    return out;
}

QDataStream& operator>>(QDataStream & in, SketchPoint & point)
{
    qint16 x;
    in >> x;
    point.setX(x);

    qint16 y;
    in >> y;
    point.setY(y);

    qint16 pressure;
    in >> pressure;
    point.setPressure(pressure);

    return in;
}

}
