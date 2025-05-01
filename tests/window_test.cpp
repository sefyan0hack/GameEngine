#include <gtest/gtest.h>
#include <core/Window.hpp>
#include <iostream>

using namespace std;
class CWindow_Test : public ::testing::Test {
    public:
    static int GetWidth(const CWindow& window) { return window.m_Width; }
    static int GetHeight(const CWindow& window) { return window.m_Height; }
};

#ifdef WINDOWS_PLT
TEST_F(CWindow_Test, ctor){
    auto r = CWindow(100, 100, "Window");

    // EXPECT_GE(GetWidth(r), 100);
    // EXPECT_LE(GetHeight(r), 100);
}
#endif