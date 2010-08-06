#include "onyx/base/scoped_ptr.h"

#include "onyx/base/base.h"
#include "gtest/gtest.h"

namespace {

using base::scoped_ptr;

TEST(ScopedPtrTest, PtrIndirection) {
    scoped_ptr<base::string> str1(new base::string("abcd"));
    EXPECT_EQ(static_cast<unsigned int>(4), str1->length());
    EXPECT_EQ("abcd", *str1);
    EXPECT_EQ(static_cast<unsigned int>(4), str1->length());
}

TEST(ScopedPtrTest, PtrBool) {
    scoped_ptr<char> ptr1;
    EXPECT_TRUE(!ptr1);
    ptr1.reset(new char('a'));
    EXPECT_TRUE(ptr1);
}

TEST(ScopedPtrTest, PtrSwap) {
    scoped_ptr<base::string> str1(new base::string("abcd"));
    scoped_ptr<base::string> str2(new base::string("qwer"));
    str1.swap(str2);
    EXPECT_EQ("qwer", *str1);
    EXPECT_EQ("abcd", *str2);
}

}  // namespace
