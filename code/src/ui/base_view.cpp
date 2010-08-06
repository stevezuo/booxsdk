#include "onyx/ui/base_view.h"

namespace vbf
{

using namespace ui;

BaseView::BaseView(QWidget *parent, Qt::WindowFlags flags)
    : QWidget(parent, flags)
{
}

BaseView::~BaseView(void)
{
}

}// namespace vbf
