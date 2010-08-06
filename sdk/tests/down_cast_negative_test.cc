//Author: Hong Jiang <hjiang@18scorpii.com>

/// This is a negative test that makes sure down_cast fails when
/// called inappropriately.

#include "onyx/base/base.h"
#include "onyx/base/down_cast.h"
#include "gtest/gtest.h"

namespace {

class Foo {
  public:
    virtual ~Foo() {}
};

class FooChild1 : public Foo {};
class FooChild2 : public Foo {};

TEST(DownCastNegativeTest, DownCast) {
    scoped_ptr<Foo> p1(new FooChild1());
    // This should fail. Casting to void to suppress compiler warning
    // about unused value.
    static_cast<void>(down_cast<FooChild2*>(p1.get()));
}

}  // anonymous namespace
