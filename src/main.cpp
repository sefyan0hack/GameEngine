#include <iostream>
#include <windows.h>
#include "Window.hpp"
#include "Renderer.hpp"
#include "Global_H.hpp"
#include "G_vars.hpp"
#include "VAO.hpp"
#include "VBO.hpp"
#include "Shader.hpp"
using namespace std;

int WINAPI WinMain(HINSTANCE Instance, HINSTANCE PrevInstance, LPSTR LpCmdLine, int NcmdShow) {
    UNUSED(Instance); UNUSED(PrevInstance); UNUSED(LpCmdLine); UNUSED(NcmdShow);
    QueryPerformanceCounter(&start_count);
    QueryPerformanceFrequency(&freq);
    while (Window::WindowsCount() != 0) {
        Window::ProcessMessages();
        TextOutA(window.GetHDC(), 20, 20, std::to_string(fps.QuadPart).c_str(), 8);
        SwapBuffers(window.GetHDC());
        Sleep(1);
        QueryPerformanceCounter(&end_count);
        counts.QuadPart = end_count.QuadPart - start_count.QuadPart;
        start_count = end_count;
        fps.QuadPart = freq.QuadPart / counts.QuadPart;
    }
    return 0;
}