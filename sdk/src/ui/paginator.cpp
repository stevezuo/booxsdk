
#include "onyx/ui/paginator.h"

Paginator::Paginator()
: first_visible_(0)
, items_per_page_(1)
, size_(0)
{
}

Paginator::~Paginator()
{
}

void Paginator::reset(int first, int items_per_page, int total)
{
    if (first >= 0)
    {
        first_visible_ = first;
    }
    else
    {
        first_visible_ = 0;
    }

    resize(items_per_page);

    if (total >= 0)
    {
        size_ = total;
    }
    else
    {
        size_ = 0;
    }
}

bool Paginator::prev()
{
    if (!isPrevEnable())
    {
        return false;
    }

    first_visible_ -= items_per_page_;
    if (first_visible_ < 0)
    {
        first_visible_ = 0;
    }
    return true;
}

bool Paginator::next()
{
    if (!isNextEnable())
    {
        return false;
    }

    first_visible_ += items_per_page_;
    if (first_visible_ > size_ - items_per_page_)
    {
        first_visible_ = size_ - items_per_page_;
    }
    return true;
}

bool Paginator::jump(int new_page)
{
    int new_pos = new_page * items_per_page_;
    if (new_pos < 0 || new_pos > size_ - items_per_page_)
    {
        return false;
    }
    first_visible_ = new_pos;
    return true;
}

bool Paginator::isPrevEnable()
{
    return (first_visible_ > 0);
}

bool Paginator::isNextEnable()
{
    return (first_visible_ < size_ - items_per_page_);
}

void Paginator::resize(int new_items_per_page)
{
    if (new_items_per_page > 0)
    {
        items_per_page_ = new_items_per_page;
    }
    else
    {
        items_per_page_ = 1;
    }
}

int Paginator::currentPage()
{
    if (items_per_page_ > 0)
    {
        return first_visible_ / items_per_page_ + 1;
    }
    return 1;
}

int Paginator::pages()
{
    if (items_per_page_ > 0)
    {
        return size_ / items_per_page_ + 1;
    }
    return 1;
}

int Paginator::last_visible()
{
    int pos = first_visible_ + items_per_page_;
    if (pos < size_)
    {
        return pos;
    }
    pos = size_ - 1;
    if (pos < 0)
    {
        return 0;
    }
    return pos;
}

int Paginator::offsetInCurrentPage()
{
    if (size_ <= 0)
    {
        return 0;
    }
    return last_visible() - first_visible() + 1;
}
