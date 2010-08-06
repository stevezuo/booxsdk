// This is a simplified version of boost::shared_ptr. This is not thread-safe.

#ifndef NABOO_BASE_SHARED_PTR_H_
#define NABOO_BASE_SHARED_PTR_H_

#include <algorithm>  // for swap
#include <cassert>

template <typename T> class shared_ptr;
template <typename T> class weak_ptr;
template <typename T> class enable_shared_from_this;

namespace internal {

class shared_ptr_counter {
  template <typename T> friend class ::shared_ptr;
  template <typename T> friend class ::weak_ptr;
 private:
  shared_ptr_counter() : refcount_(1), weak_count_(1) { }
  int refcount_;
  int weak_count_;
};

template <typename T1, typename T2, typename T3>
T2 CompareAndSet(T1* n, T2 m, T3 v) {
  T2 r = *n;
  if (r == m) *n = v;
  return r;
}

}  // namespace internal

template <typename T>
class shared_ptr {
  template <typename U> friend class weak_ptr;
 public:
  typedef T element_type;

  explicit shared_ptr(T* ptr = NULL)
      : ptr_(ptr),
        counter_(ptr != NULL ? new internal::shared_ptr_counter : NULL) {
    sp_enable_shared_from_this(counter_, ptr_, ptr);
  }

  template <typename U>
  shared_ptr(const shared_ptr<U>& ptr)
      : ptr_(NULL),
        counter_(NULL) {
    Initialize(ptr);
  }
  // Prevent default constructor
  shared_ptr(const shared_ptr<T>& ptr)
      : ptr_(NULL),
        counter_(NULL) {
    Initialize(ptr);
  }

  template <typename U>
  shared_ptr<T>& operator=(const shared_ptr<U>& ptr) {
    if (ptr_ != ptr.ptr_) {
      shared_ptr<T> me(ptr);
      swap(me);
    }
    return *this;
  }

  // Prevent default assignment
  shared_ptr<T>& operator=(const shared_ptr<T>& ptr) {
    if (ptr_ != ptr.ptr_) {
      shared_ptr<T> me(ptr);
      swap(me);
    }
    return *this;
  }

  ~shared_ptr() {
    if (ptr_ != NULL) {
      if ((--(counter_->refcount_)) == 0) {
        delete ptr_;

        if ((--(counter_->weak_count_)) == 0) {
          delete counter_;
        }
      }
    }
  }

  // enable_shared_from_this support
  template <typename X, typename Y>
  static void sp_enable_shared_from_this(
      internal::shared_ptr_counter* counter,
      const enable_shared_from_this<X> * pe,
      Y* px) {
    if(pe != 0) pe->weak_this_.CopyFrom(px, counter);
  }

  static void sp_enable_shared_from_this(
      const internal::shared_ptr_counter* counter, ...) {
  }

  void reset(T* p = NULL) {
    if (p != ptr_) {
      shared_ptr<T> tmp(p);
      tmp.swap(*this);
    }
  }

  void swap(shared_ptr<T>& r) {
    std::swap(ptr_, r.ptr_);
    std::swap(counter_, r.counter_);
  }

  T* get() const {
    return ptr_;
  }

  T& operator*() const {
    return *ptr_;
  }

  T* operator->() const {
    return ptr_;
  }

  long use_count() const {
    return counter_ ? counter_->refcount_ : 1;
  }

  bool unique() const {
    return use_count() == 1;
  }

  operator bool () const
  {
    return ptr_ != 0;
  }

 private:
  template <typename U>
  void Initialize(const shared_ptr<U>& r) {
    if (r.counter_ != NULL) {
      ++(r.counter_->refcount_);

      ptr_ = r.ptr_;
      counter_ = r.counter_;
    }
  }

  T* ptr_;
  internal::shared_ptr_counter* counter_;

  template <typename U>
  friend class shared_ptr;
};

template<class T, class U> inline bool operator==(shared_ptr<T> const & a, shared_ptr<U> const & b) {
  return a.get() == b.get();
}

template<class T, class U> inline bool operator!=(shared_ptr<T> const & a, shared_ptr<U> const & b) {
  return a.get() != b.get();
}

template <typename T> void swap(shared_ptr<T>& r, shared_ptr<T>& s) {
  r.swap(s);
}

template <typename T>
class weak_ptr {
  template <typename U> friend class weak_ptr;
  template<typename X, typename Y>
  friend void sp_enable_shared_from_this(
      internal::shared_ptr_counter* counter,
      const enable_shared_from_this<X> * pe,
      Y* px);
 public:
  typedef T element_type;

  weak_ptr() : ptr_(NULL), counter_(NULL) { }

  template <typename U> weak_ptr(const shared_ptr<U>& ptr) {
    CopyFrom(ptr.ptr_, ptr.counter_);
  }

  template <typename U> weak_ptr(const weak_ptr<U>& ptr) {
    CopyFrom(ptr.ptr_, ptr.counter_);
  }

  // Prevent default constructor.
  weak_ptr(const weak_ptr& ptr) {
    CopyFrom(ptr.ptr_, ptr.counter_);
  }

  ~weak_ptr() {
    if (counter_ != NULL &&
        (--(counter_->weak_count_)) == 0) {
      delete counter_;
    }
  }

  weak_ptr& operator=(const weak_ptr& ptr) {
    if (&ptr != this) {
      weak_ptr tmp(ptr);
      tmp.swap(*this);
    }
    return *this;
  }
  template <typename U> weak_ptr& operator=(const weak_ptr<U>& ptr) {
    weak_ptr tmp(ptr);
    tmp.swap(*this);
    return *this;
  }
  template <typename U> weak_ptr& operator=(const shared_ptr<U>& ptr) {
    weak_ptr tmp(ptr);
    tmp.swap(*this);
    return *this;
  }

  void swap(weak_ptr& ptr) {
    std::swap(ptr_, ptr.ptr_);
    std::swap(counter_, ptr.counter_);
  }

  void reset() {
    weak_ptr tmp;
    tmp.swap(*this);
  }

  // Return the number of shared_ptrs that own the object we are observing.
  // Note that this number can be 0 (if this pointer has expired).
  long use_count() const {
    return counter_ != NULL ? counter_->refcount_ : 0;
  }

  bool expired() const { return use_count() == 0; }

  shared_ptr<T> lock() const {
    shared_ptr<T> result;
    if (counter_ != NULL) {
      int old_refcount;
      do {
        old_refcount = counter_->refcount_;
        if (old_refcount == 0)
          break;
      } while (old_refcount !=
               internal::CompareAndSet(&counter_->refcount_,
                                       old_refcount,
                                       old_refcount + 1));
      if (old_refcount > 0) {
        result.ptr_ = ptr_;
        result.counter_ = counter_;
      }
    }

    return result;
  }

 private:
  void CopyFrom(T* ptr, internal::shared_ptr_counter* control_block) {
    ptr_ = ptr;
    counter_ = control_block;
    if (counter_ != NULL)
      ++(counter_->weak_count_);
  }

 private:
  element_type* ptr_;
  internal::shared_ptr_counter* counter_;
};

template <typename T> void swap(weak_ptr<T>& r, weak_ptr<T>& s) {
  r.swap(s);
}

template<typename T> class enable_shared_from_this {
  template<typename X, typename Y>
  friend void sp_enable_shared_from_this(
      internal::shared_ptr_counter* counter,
      const enable_shared_from_this<X> * pe,
      Y* px);
 public:
  shared_ptr<T> shared_from_this() {
    shared_ptr<T> p(weak_this_.lock());
    assert(p.get() == this);
    return p;
  }

  shared_ptr<T const> shared_from_this() const {
    shared_ptr<T const> p(weak_this_.lock());
    assert(p.get() == this);
    return p;
  }

 protected:
  enable_shared_from_this() {}

  enable_shared_from_this(enable_shared_from_this const &) {}

  enable_shared_from_this & operator=(enable_shared_from_this const &) {
    return *this;
  }

  ~enable_shared_from_this() {}

 private:
  // This is initialized by a shared_ptr that is initialized from a raw
  // pointer.
  mutable weak_ptr<T> weak_this_;
};

#endif  // #ifndef NABOO_BASE_SHARED_PTR_H_
