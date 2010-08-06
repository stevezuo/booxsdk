// Author: Hong Jiang <hjiang@18scorpii.com>

#ifndef BASE_DOWN_CAST_H__
#define BASE_DOWN_CAST_H__

#include <stdlib.h>

#include "onyx/base/macros.h"
#include "onyx/base/scoped_ptr.h"

/// Never use dynamic_cast. If you must do down casting, use
/// this. down_cast only does run-time type checking in debug mode, so
/// it won't cause a performance penalty in optimized code. But still,
/// don't use it. The need for down casting implies bad design.
template <typename To, typename From>
inline To down_cast(From* ptr) {
    DCHECK(dynamic_cast<To>(ptr));
    return static_cast<To>(ptr);
}

template <typename To, typename From>
inline To down_cast(base::scoped_ptr<From>& ptr) {
    return down_cast<To>(ptr.get());
}

#endif  // BASE_DOWN_CAST_H__
