//Author: Hong Jiang <hjiang@18scorpii.com>

#include "onyx/base/down_cast.h"

#include "onyx/base/base.h"
#include "gtest/gtest.h"

namespace {

class Foo {
  public:
    virtual ~Foo() {}
};

class FooChild : public Foo {};

TEST(DownCastTest, DownCast) {
    scoped_ptr<Foo> p1(new FooChild());
    FooChild* p2 = down_cast<FooChild*>(p1.get());
    EXPECT_FALSE(NULL == p2);
}

}  // anonymous namespace
