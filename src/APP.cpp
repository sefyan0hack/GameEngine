#include "APP.hpp"
#include "Window.hpp"

APP::APP(): m_Window(640, 480, "Main"), fps(LARGE_INTEGER{.QuadPart = 60}) {}

void APP::Run()
{
    QueryPerformanceCounter(&start_count);
    QueryPerformanceFrequency(&freq);
    while (Window::WindowsCount() != 0) {
        Window::ProcessMessages();
        auto _hdc = m_Window.GetGL()->GetHDC();
        glViewport(0, 0, m_Window.GetWidth(), m_Window.GetHeight());
        glClearColor(0.2f, 0.21f, 0.22f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Update(1/static_cast<float>(fps.QuadPart));

        SwapBuffers(_hdc);
        Sleep(1);
        QueryPerformanceCounter(&end_count);
        counts.QuadPart = end_count.QuadPart - start_count.QuadPart;
        start_count = end_count;
        fps.QuadPart = freq.QuadPart / counts.QuadPart;
    }
}
