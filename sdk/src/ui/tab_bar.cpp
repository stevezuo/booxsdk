#include "onyx/ui/tab_bar.h"

namespace ui
{

TabBar::TabBar(QWidget *parent)
: QWidget(parent)
, layout_(QBoxLayout::LeftToRight, this)
, orientation_(Qt::Horizontal)
, buttons_()
{
    InitLayout();
}

TabBar::~TabBar(void)
{
    Clear();
}

void TabBar::InitLayout()
{
    layout_.setSpacing(0);
    layout_.setContentsMargins(0,0,0,0);
}

bool TabBar::AddButton(const int id,
                       const QString & title,
                       const QIcon & icon)
{
    TabButton * button = new TabButton(this, id);
    button->setText(title);
    button->setIcon(icon);
    layout_.addWidget(button);
    buttons_.push_back(button);

    connect(button, SIGNAL(clicked(const int, bool)),
            this, SLOT(OnClicked(const int, bool)));

    return true;
}

bool TabBar::RemoveButton(const int id)
{
    return false;
}

bool TabBar::ClickButton(const int id)
{
    for(ButtonIter i = buttons_.begin(); i != buttons_.end(); ++i)
    {
        if ((*i)->id() == id)
        {
            (*i)->click();
            return true;
        }
    }
    return false;
}

int TabBar::GetSelectedButton()
{
    for(ButtonIter i = buttons_.begin(); i != buttons_.end(); ++i)
    {
        if ((*i)->isChecked())
        {
            return (*i)->id();
        }
    }
    return -1;
}

bool TabBar::SetButtonText(const int id, const QString & title)
{
    for(ButtonIter i = buttons_.begin(); i != buttons_.end(); ++i)
    {
        if ((*i)->id() == id)
        {
            (*i)->setText(title);
            return true;
        }
    }
    return false;

}

bool TabBar::SetOrientation(const Qt::Orientation orientation)
{
    if (orientation == orientation_)
    {
        return false;
    }

    // Move widgets from one layout to the other.
    orientation_ = orientation;
    if (orientation_ == Qt::Horizontal)
    {
        layout_.setDirection(QBoxLayout::LeftToRight);
    }
    else
    {
        layout_.setDirection(QBoxLayout::TopToBottom);
    }
    return true;
}

void TabBar::mouseDoubleClickEvent(QMouseEvent *)
{
    if (orientation_ == Qt::Horizontal)
    {
        SetOrientation(Qt::Vertical);
    }
    else
    {
        SetOrientation(Qt::Horizontal);
    }
}

void TabBar::keyPressEvent(QKeyEvent *)
{
}

void TabBar::keyReleaseEvent(QKeyEvent *e)
{
    switch (e->key())
    {
    // May depends on the layout orientation.
    case Qt::Key_Left:
        SetFocusNextPrevChild(false);
        break;
    case Qt::Key_Right:
        SetFocusNextPrevChild(true);
        break;
    case Qt::Key_Return:
        ClickSelectedChild();
        break;
    default:
        break;
    }
}

void TabBar::OnClicked(const int id, bool check)
{
    for(ButtonIter iter = buttons_.begin(); iter != buttons_.end(); ++iter)
    {
        if (id != (*iter)->id())
        {
            (*iter)->setChecked(false);
        }
        else
        {
            (*iter)->setChecked(true);
        }
    }
    emit ButtonClicked(id);
}

void TabBar::Clear()
{
    for(ButtonIter iter = buttons_.begin(); iter != buttons_.end(); ++iter)
    {
        layout_.removeWidget(*iter);
        delete *iter;
    }
    buttons_.clear();
}

/// Set focus to next or prev child according to the \next.
void TabBar::SetFocusNextPrevChild(bool next)
{
    // Query which one has the focus.
    int pos = 0;
    int size = static_cast<int>(buttons_.size());
    for(; pos != size; ++pos)
    {
        if (buttons_[pos]->hasFocus())
        {
            break;
        }
    }

    if (next && pos < size - 1)
    {
        ++pos;
        buttons_[pos]->setFocus();
    }
    else if (!next && pos > 0)
    {
        --pos;
        buttons_[pos]->setFocus();
    }
}

void TabBar::ClickSelectedChild()
{
    ButtonIter iter = buttons_.begin();
    for(; iter != buttons_.end(); ++iter)
    {
        if ((*iter)->hasFocus())
        {
            (*iter)->setChecked(true);
        }
        else
        {
            (*iter)->setChecked(false);
        }
    }
}

}
