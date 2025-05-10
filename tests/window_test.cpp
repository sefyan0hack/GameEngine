#include <gtest/gtest.h>
#include <core/Window.hpp>
#include <iostream>
#ifdef WINDOWS_PLT
#include <windows.h>
using namespace std;
// class CWindow_Test : public ::testing::Test {
//     public:
//     static auto Hinstance(const CWindow& w)     -> HINSTANCE { return w.m_Instance; }
//     static auto WindowHandle(const CWindow& w)  -> HWND      { return w.m_WindowHandle; }
//     static auto DrawContext(const CWindow& w)   -> HDC       { return w.m_HDC; }
//     static auto Width(const CWindow& w)         -> int       { return w.m_Width; }
//     static auto Height(const CWindow& w)        -> int       { return w.m_Height; }
//     static auto opengl(const CWindow& w)        -> std::shared_ptr<OpenGL> { return w.m_OpenGl; }
//     static auto Visible(const CWindow& w)       -> bool { return w.m_Visible; }
//     static auto WindowsCount()                  -> unsigned short { return CWindow::S_WindowsCount; }
// };

// TEST_F(CWindow_Test, ctor){
//     auto r = CWindow(100, 100, "Window");

//     EXPECT_NE(Hinstance(r), nullptr);
//     EXPECT_NE(WindowHandle(r), nullptr);
//     EXPECT_NE(DrawContext(r), nullptr);
//     EXPECT_EQ(opengl(r)->isValid(), true);
//     EXPECT_EQ(Visible(r), false);
//     EXPECT_EQ(WindowsCount(), 1);
// }
#endif