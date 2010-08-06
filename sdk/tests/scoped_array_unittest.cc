#include "onyx/base/scoped_array.h"

#include "onyx/base/base.h"
#include "gtest/gtest.h"

namespace {

using base::scoped_array;

TEST(ScopedArrayTest, ArrayAccess) {
    scoped_array<int> a1(new int[6]);
    a1[0] = 5;
    a1[5] = 2;
    EXPECT_EQ(5, a1[0]);
    EXPECT_EQ(2, a1[5]);
}

TEST(ScopedArrayTest, ArrayBool) {
    scoped_array<char> a1;
    EXPECT_TRUE(!a1);
    a1.reset(new char[5]);
    EXPECT_TRUE(a1);
}

TEST(ScopedArrayTest, PtrSwap) {
    scoped_array<int> a1(new int[6]);
    a1[0] = 5;
    a1[5] = 2;
    scoped_array<int> a2(new int[3]);
    a2[0] = 77;
    a2[2] = 66;
    a1.swap(a2);
    EXPECT_EQ(2, a2[5]);
    EXPECT_EQ(66, a1[2]);
}

}  // namespace
