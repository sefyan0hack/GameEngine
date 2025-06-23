#include <gtest/gtest.h>
#include <core/Buffer.hpp>

using namespace std;

using Buffer_Test_Ctor1 = Buffer::Test<1>;

#define TEST_INTERFACE \
    MEMBER_VAR(m_Size);\
    MEMBER_VAR(m_Data);\
    MEMBER_VAR(m_Cursor);\
    \
    MEMBER_FUN(operator<<);\
    MEMBER_FUN(operator==);\
    MEMBER_FUN(operator!=);\
    MEMBER_OPCAST(const uint8_t*)\
    MEMBER_OPCAST(const int8_t*)\
    \
    MEMBER_FUN(clone);\
    MEMBER_FUN(write);\
    MEMBER_FUN(push);\
    MEMBER_FUN(data);\
    MEMBER_FUN(size);\
    MEMBER_FUN(empty);


template<> 
struct Buffer::Test<1> : public ::testing::Test {
    Buffer member;
    Test() : member(10) {}
    TEST_INTERFACE
};

TEST_F(Buffer_Test_Ctor1, ctor) {
    EXPECT_EQ(m_Size, 10);
    EXPECT_NE(m_Data, nullptr);
    EXPECT_EQ(m_Cursor, 0);
}
