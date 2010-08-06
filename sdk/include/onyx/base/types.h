// Authors: Hong Jiang <hjiang@dev-gems.com>

#ifndef BASE_TYPES_H__
#define BASE_TYPES_H__

// ‘static_assert’ will become a keyword in C++0x

namespace base {

struct true_type {
};

struct false_type {
};

template <typename T>
struct my_static_assert;  // intentionally undefined.

template <>
struct my_static_assert<true_type> {};

template <typename T>
struct my_static_assert_not;  // intentionally undefined.

template <>
struct my_static_assert_not<false_type> {};

}  // namespace base

#endif  // BASE_TYPES_H__
