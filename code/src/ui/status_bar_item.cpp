#include "onyx/ui/status_bar_item.h"

namespace ui
{

StatusBarItem::StatusBarItem(const StatusBarItemType type, QWidget *parent)
    : QWidget(parent)
    , type_(type)
    , state_(STATE_NORMAL)
{
}

StatusBarItem::~StatusBarItem(void)
{
}


void StatusBarItem::setState(const int state)
{
    if (state_ == state)
    {
        return;
    }
    state_ = state;
}

}
