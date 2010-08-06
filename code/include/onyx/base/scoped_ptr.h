#ifndef BASE_SCOPED_PTR_H__
#define BASE_SCOPED_PTR_H__

//  (C) Copyright Greg Colvin and Beman Dawes 1998, 1999.
//  Copyright (c) 2001, 2002 Peter Dimov
//  Modified by Onyx International Inc.
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//  http://www.boost.org/libs/smart_ptr/scoped_ptr.htm
//

#include <cassert>

namespace base
{
///  scoped_ptr mimics a built-in pointer except that it guarantees deletion
///  of the object pointed to, either on destruction of the scoped_ptr or via
///  an explicit reset(). scoped_ptr is a simple solution for simple needs;
///  use shared_ptr or std::auto_ptr if your needs are more complex.
template<class T> class scoped_ptr // noncopyable
{
private:

    T * ptr_;

    scoped_ptr(scoped_ptr const &);
    scoped_ptr & operator=(scoped_ptr const &);

    typedef scoped_ptr<T> this_type;

public:

    typedef T element_type;

    explicit scoped_ptr(T * p = 0): ptr_(p) // never throws
    {
    }

    ~scoped_ptr() // never throws
    {
        delete ptr_;
    }

    void reset(T * p = 0) // never throws
    {
        assert(p == 0 || p != ptr_);
        // This is not exception-safe, but we don't use exception.
        delete ptr_;
        ptr_ = p;
    }

    T & operator*() const // never throws
    {
        assert(ptr_ != 0);
        return *ptr_;
    }

    T * operator->() const // never throws
    {
        assert(ptr_ != 0);
        return ptr_;
    }

    T * get() const // never throws
    {
        return ptr_;
    }

    T* release() {
        T* t = ptr_;
        ptr_ = 0;
        return t;
    }

    // implicit conversion to "bool"
    typedef T * this_type::*unspecified_bool_type;

    operator unspecified_bool_type() const // never throws
    {
        return ptr_ == 0? 0: &this_type::ptr_;
    }

    bool operator! () const // never throws
    {
        return ptr_ == 0;
    }

    void swap(scoped_ptr & b) // never throws
    {
        T * tmp = b.ptr_;
        b.ptr_ = ptr_;
        ptr_ = tmp;
    }
};

template<class T> inline void swap(scoped_ptr<T> & a, scoped_ptr<T> & b) // never throws
{
    a.swap(b);
}

// get_pointer(p) is a generic way to say p.get()

template<class T> inline T * get_pointer(scoped_ptr<T> const & p)
{
    return p.get();
}

} // namespace base

#endif // #ifndef BASE_SCOPED_PTR_H__
