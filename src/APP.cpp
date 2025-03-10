#include <core/APP.hpp>
#include <core/Window.hpp>
#include <tchar.h>

APP::APP()
: m_Window(1180, 640, _T("Main"))
, start_count({})
, end_count({})
, counts({})
, freq({})
, fps(LARGE_INTEGER{.QuadPart = 60}) {}

auto APP::Run() -> void
{
    QueryPerformanceCounter(&start_count);
    QueryPerformanceFrequency(&freq);
    glClearColor(0.2f, 0.21f, 0.22f, 1.0f);
    
    while (Window::WindowsCount() != 0) {
        Window::ProcessMessages();
        auto _hdc = m_Window.opengl()->DrawContext();
        glViewport(0, 0, m_Window.Width(), m_Window.Height());
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
