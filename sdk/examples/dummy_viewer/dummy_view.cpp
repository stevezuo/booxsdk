#include "dummy_view.h"

namespace dummy
{

using namespace ui;

DummyView::DummyView(Widget *parent)
    : Widget(parent, 0)
{
}

DummyView::~DummyView(void)
{
}

void DummyView::SetPath(const QString &path)
{
    path_ = path;
    update();
}

void DummyView::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.drawText(QPoint(100, 100), path_);
}

}// namespace dummy
