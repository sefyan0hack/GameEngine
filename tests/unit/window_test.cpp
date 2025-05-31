#include <gtest/gtest.h>
#include <core/Window.hpp>

using namespace std;

using CWindow_Test_Ctor1 = CWindow::Test<1>;

#define TEST_INTERFACE \
    MEMBER_VAR(m_WindowHandle);\
    MEMBER_VAR(m_DrawContext);\
    MEMBER_VAR(m_Width);\
    MEMBER_VAR(m_Height);\
    MEMBER_VAR(m_Visible);\
    MEMBER_VAR(m_RawBuffer);\
    MEMBER_VAR(m_OpenGl);\
    MEMBER_VAR(m_refCount);\
    MEMBER_VAR(m_Keyboard);\
    MEMBER_VAR(m_Mouse);\
    MEMBER_VAR(S_WindowsCount);\
    \
    MEMBER_FUN(WindowHandle);\
    MEMBER_FUN(DrawContext);\
    MEMBER_FUN(Width);\
    MEMBER_FUN(Height);\
    MEMBER_FUN(opengl);\
    MEMBER_FUN(Visible);\
    MEMBER_FUN(Show);\
    MEMBER_FUN(Hide);\
    MEMBER_FUN(WindowsCount);


template<>
struct CWindow::Test<1> : public ::testing::Test {
    CWindow member;
    Test() : member(CWindow(100, 100, "Window")){}
    TEST_INTERFACE
};

TEST_F(CWindow_Test_Ctor1, ctor){

    EXPECT_NE(m_WindowHandle, nullptr);
    EXPECT_NE(m_DrawContext, nullptr);
    EXPECT_EQ(m_OpenGl->isValid(), true);
    EXPECT_EQ(m_Visible, false);
    EXPECT_EQ(S_WindowsCount, 1);
}