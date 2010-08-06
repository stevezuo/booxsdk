// -*- mode: c++; c-basic-offset: 4; -*-

// Author: Hong Jiang <hjiang@onyx-international.com>
/// This file defines a few useful macros.

#ifndef BASE_MACROS_H__
#define BASE_MACROS_H__

#include <stdlib.h>
#include <iostream>

#define CHECK(condition)                                   \
    do {                                                   \
        if (!(condition)) {                                \
            std::cerr << __FILE__ << ": " << __LINE__      \
                      << ": CHECK FAILED: " #condition " " \
                      << std::endl;                        \
            exit(EXIT_FAILURE);                            \
        }                                                  \
    } while (false)

// NOTE: DCHECK(statement) does nothing in optimized code. \statement
// is not executed!
#ifdef NDEBUG
#define DCHECK(condition)                       \
    do {                                        \
    } while (false)
#else
#define DCHECK(condition)                                \
    do {                                                 \
        if (!(condition)) {                              \
            std::cerr << __FILE__ << ": " << __LINE__      \
                      << ": CHECK FAILED: " #condition " " \
                      << std::endl;                      \
            exit(EXIT_FAILURE);                          \
        }                                                \
    } while (false)
#endif

// A macro to disallow copy and assignment.
// This should be used in the private: declarations for a class.
#define NO_COPY_AND_ASSIGN(TypeName)    \
  private:                                    \
  TypeName(const TypeName&);                  \
  void operator=(const TypeName&)

// A macro to disallow all the implicit constructors, namely the
// default constructor, copy constructor and operator= functions.
//
// This should be used in the private: declarations for a class
// that wants to prevent anyone from instantiating it. This is
// especially useful for classes containing only static methods.
#define NO_IMPLICIT_CONSTRUCTORS(TypeName) \
  TypeName();                              \
  NO_COPY_AND_ASSIGN(TypeName)

#endif  // BASE_MACROS_H__
