
#ifndef DISPLAY_PAGES_CONTAINER_H
#define DISPLAY_PAGES_CONTAINER_H

#include "onyx/base/base.h"
#include "onyx/ui/ui_global.h"

using namespace ui;
using namespace base;

namespace vbf
{

// container of all displaying pages
template <class ValueType>
class DisplayPages
{
public:
    DisplayPages()
        : pages_() {}
    ~DisplayPages() {}

    void push_back(shared_ptr<ValueType> p);
    void clear();
    size_t size();
    shared_ptr<ValueType> get_page(int key);

private:
    typedef QVector<shared_ptr<ValueType> > Pages;
    typedef typename Pages::iterator  PagesIter;

    Pages pages_;
};

template <class ValueType>
void DisplayPages<ValueType>::push_back(shared_ptr<ValueType> p)
{
    p->lock();
    pages_.push_back(p);
}

template <class ValueType>
void DisplayPages<ValueType>::clear()
{
    for (PagesIter it = pages_.begin(); it != pages_.end(); ++it)
    {
        (*it)->unlock();
    }
    pages_.clear();
}

template <class ValueType>
size_t DisplayPages<ValueType>::size()
{
    return pages_.size();
}

template <class ValueType>
shared_ptr<ValueType> DisplayPages<ValueType>::get_page(int key)
{
    return pages_[key];
}

};

#endif
