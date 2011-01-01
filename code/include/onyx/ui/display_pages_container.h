
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
    void push_front(shared_ptr<ValueType> p);
    void clear();
    size_t size();
    shared_ptr<ValueType> get_page(int key);
    shared_ptr<ValueType> last();
    shared_ptr<ValueType> front();

    DisplayPages<ValueType> & operator=(const DisplayPages<ValueType> & other);

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
void DisplayPages<ValueType>::push_front(shared_ptr<ValueType> p)
{
    p->lock();
    pages_.push_front(p);
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

template <class ValueType>
DisplayPages<ValueType> & DisplayPages<ValueType>::operator=(const DisplayPages<ValueType> & other)
{
    this->pages_ = other.pages_;
    return *this;
}

template <class ValueType>
shared_ptr<ValueType> DisplayPages<ValueType>::last()
{
    return pages_.last();
}

template <class ValueType>
shared_ptr<ValueType> DisplayPages<ValueType>::front()
{
    return pages_.front();
}

};

#endif
