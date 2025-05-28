#include <gtest/gtest.h>
#include <core/AutoRelease.hpp>

using namespace std;

using SubType = int;

using AutoRelease_Test_Ctor1 = AutoRelease<SubType>::Test<1>;

#define INTERFACE \
    MEMBER_VAR(resource);\
    MEMBER_VAR(deleter);\
    MEMBER_VAR(ref_count);\
    \
    MEMBER_FUN(operator=);\
    MEMBER_FUN(operator==);\
    MEMBER_FUN(release);\
    MEMBER_FUN(get);\
    MEMBER_FUN(use_count);


template<> template<>
struct AutoRelease<SubType>::Test<1> : public ::testing::Test {
    AutoRelease<SubType> member;
    Test() : member(10, [](auto&& x){ cout << "delete  x : " << x; }) {}
    INTERFACE
};

TEST_F(AutoRelease_Test_Ctor1, ctor) {
    EXPECT_EQ(resource, 10);
    EXPECT_NE(deleter, nullptr);
}