#include "onyx/data/sketch_page.h"

namespace sketch
{

SketchPage::SketchPage()
  : orient_(ROTATE_0_DEGREE)
  , bk_color_(255, 255, 255)
  , content_area_(0, 0, 600, 800)
  , id_(-1)
  , key_()
  , timestamp_()
  , is_data_dirty_(false)
  , is_key_dirty_(false)
  , is_background_dirty_(false)
  , data_loaded_(false)
  , strokes_()
  , last_erase_point_()
{
}

SketchPage::~SketchPage(void)
{
    clearStrokes();
}

void SketchPage::setID(const int id)
{
    id_ = id;
}

void SketchPage::setKey(const PageKey & k)
{
    key_ = k;
}

void SketchPage::setOrient(const RotateDegree r)
{
    orient_ = r;
}

void SketchPage::setContentArea(const QRect & a)
{
    content_area_ = a;
}

void SketchPage::setDisplayArea(const QRect & d)
{
    display_area_ = d;
}

void SketchPage::appendStroke(SketchStrokePtr stroke)
{
    strokes_.append(stroke);
}

void SketchPage::clearStrokes()
{
    strokes_.clear();
}

void SketchPage::removeStrokes(const Strokes & strokes)
{
    foreach (SketchStrokePtr ptr, strokes)
    {
        int idx = strokes_.indexOf(ptr);
        if (idx >= 0)
        {
            strokes_.remove(idx);
        }
    }
}

int SketchPage::getStrokeCount()
{
    return strokes_.size();
}

bool SketchPage::hitTest(QMouseEvent *e, SketchPosition &position)
{
    if (display_area_.contains(e->pos()))
    {
        position.widget_pos = e->pos();
        position.global_pos = e->globalPos();
        position.sketch_pos = e->pos() - display_area_.topLeft();
        return true;
    }
    return false;
}

bool SketchPage::hitTest(const QPoint & p,
                         EraseContext & ctx,
                         Strokes & strokes)
{
    bool ret = false;
    strokes.clear();
    if (ctx.last_point_.isValid())
    {
        QLine line(ctx.last_point_, p);
        ret = hitTestStrokes(line, ctx, strokes);
        ctx.last_point_ = p;
    }
    else
    {
        ret = hitTestStrokes(p, ctx, strokes);
        ctx.last_point_ = p;
    }

    return ret;
}

bool SketchPage::hitTestStrokes(const QPoint & p,
                                const EraseContext & ctx,
                                Strokes & strokes)
{
    foreach (SketchStrokePtr ptr, strokes_)
    {
        if (ptr->hitTest(p, ctx))
        {
            strokes.append(ptr);
        }
    }
    return (!strokes.empty());
}

bool SketchPage::hitTestStrokes(const QLine & line,
                                const EraseContext & ctx,
                                Strokes & strokes)
{
    foreach (SketchStrokePtr ptr, strokes_)
    {
        if (ptr->hitTest(line, ctx))
        {
            strokes.append(ptr);
        }
    }
    return (!strokes.empty());
}

int SketchPage::length()
{
    int len = getLengthOfAttributes();

    foreach (SketchStrokePtr ptr, strokes_)
    {
        len += ptr->length();
    }

    return len;
}

int SketchPage::getLengthOfAttributes()
{
    // the length of strokes is also one attribute
    return (sizeof(orient_) +
            sizeof(bk_color_) +
            sizeof(content_area_) +
            sizeof(int));
}

void SketchPage::paint(const SketchContext & sketch_ctx,
                       GraphicContext & gc,
                       QPainter & painter)
{
    foreach (SketchStrokePtr ptr, strokes())
    {
        ptr->paint(sketch_ctx, display_area_, gc, painter);
    }
}

QDataStream& operator<<(QDataStream & out, const SketchPage & page)
{
    if (page.dumpAttributes(out))
    {
        page.dumpStrokes(out);
    }
    return out;
}

bool SketchPage::dumpAttributes(QDataStream & out) const
{
    out << static_cast<qint16>(orient_);
    out << bk_color_;
    out << content_area_;
    out << static_cast<qint16>(strokes_.size());
    return true;
}

bool SketchPage::dumpStrokes(QDataStream & out) const
{
    foreach (SketchStrokePtr ptr, strokes_)
    {
        out << *ptr;
    }

    return true;
}

QDataStream& operator>>(QDataStream & in, SketchPage & page)
{
    int num_strokes = 0;
    if (page.loadAttributes(in, num_strokes))
    {
        page.loadStrokes(in, num_strokes);
    }
    page.setDataLoaded(true);
    return in;
}

bool SketchPage::loadAttributes(QDataStream & in, int & num_strokes)
{
    qint16 orientation;
    in >> orientation;
    orient_ = static_cast<RotateDegree>(orientation);

    in >> bk_color_;
    in >> content_area_;

    qint16 num;
    in >> num;
    num_strokes = num;

    return true;
}

bool SketchPage::loadStrokes(QDataStream & in, const int num_strokes)
{
    for (int idx = 0; idx < num_strokes; ++idx)
    {
        SketchStrokePtr stroke(new SketchStroke());
        in >> *stroke;
        appendStroke(stroke);
    }

    return true;
}

}
