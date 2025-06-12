#include <gtest/gtest.h>
#include <core/AutoRelease.hpp>

using namespace std;

using SubType = int32_t;

using AutoRelease_Test_Ctor1 = AutoRelease<SubType>::Test<1>;

#define TEST_INTERFACE \
    MEMBER_VAR(resource);\
    MEMBER_VAR(deleter);\
    MEMBER_VAR(ref_count);\
    \
    MEMBER_OPCAST(SubType)\
    MEMBER_FUN(operator=);\
    MEMBER_FUN(operator==);\
    MEMBER_FUN(release);\
    MEMBER_FUN(get);\
    MEMBER_FUN(use_count);


template<> template<>
struct AutoRelease<SubType>::Test<1> : public ::testing::Test {
    AutoRelease<SubType> member;
    Test() : member(10, [](auto&& x){ x = 0; }) {}
    TEST_INTERFACE
};

TEST_F(AutoRelease_Test_Ctor1, ctor) {
    EXPECT_EQ(resource, 10);
    EXPECT_NE(deleter, nullptr);
    EXPECT_NE(*ref_count, 0);
}

TEST_F(AutoRelease_Test_Ctor1, operator_eq) {
    AutoRelease<SubType> member2 = member;
    EXPECT_EQ(member, member2);
}

TEST_F(AutoRelease_Test_Ctor1, operator_cmp_eq) {
    auto use_count = member.use_count();
    AutoRelease<SubType> member2 = member;
    EXPECT_EQ(*ref_count, use_count + 1);
}

TEST_F(AutoRelease_Test_Ctor1, get) {
    auto getstr = ::type_name<decltype(member.get())>();
    auto expectedstr = ::type_name<SubType>();
    EXPECT_EQ(getstr, expectedstr);
}

TEST_F(AutoRelease_Test_Ctor1, operator_Cast) {
    EXPECT_EQ(member, 10);
}

TEST_F(AutoRelease_Test_Ctor1, release) {
    auto use_count = member.use_count();
    member.release();
    EXPECT_EQ(*ref_count, use_count - 1);
}

TEST_F(AutoRelease_Test_Ctor1, use_count) {
    EXPECT_GE(member.use_count(), 0);
}