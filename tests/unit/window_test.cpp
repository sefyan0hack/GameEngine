#include <gtest/gtest.h>
#include <core/Window.hpp>

using namespace std;

using CWindow_Test_Ctor1 = CWindow::Test<1>;

#define TEST_INTERFACE \
    MEMBER_VAR(m_Handle);\
    MEMBER_VAR(m_Surface);\
    MEMBER_VAR(m_Width);\
    MEMBER_VAR(m_Height);\
    MEMBER_VAR(m_Visible);\
    MEMBER_VAR(m_OpenGl);\
    MEMBER_VAR(m_EventQueue);\
    \
    MEMBER_FUN(Handle);\
    MEMBER_FUN(Surface);\
    MEMBER_FUN(Width);\
    MEMBER_FUN(Height);\
    MEMBER_FUN(opengl);\
    MEMBER_FUN(Visible);\
    MEMBER_FUN(Show);\
    MEMBER_FUN(Hide);\


// template<>
// struct CWindow::Test<1> : public ::testing::Test {
//     CWindow member;
//     Test() : member(CWindow(100, 100, "Window", false)){}
//     TEST_INTERFACE
// };

// TEST_F(CWindow_Test_Ctor1, ctor){
//     #if defined(WINDOWS_PLT)
//     EXPECT_NE(m_Handle, nullptr);
//     EXPECT_EQ(IsWindow(m_Handle), true);
//     #elif defined(LINUX_PLT)
//     //check in X11 if m_Handle is valid :)
//     XWindowAttributes attr{};
//     Status status = XGetWindowAttributes(m_Surface, m_Handle, &attr);
//     EXPECT_NE(status, 0);
//     #endif
//     EXPECT_NE(m_Surface, nullptr);
//     EXPECT_EQ(m_Visible, false);
// }