#include <core/APP.hpp>
#include <core/Window.hpp>
#include <tchar.h>

APP::APP()
: m_Window(1180, 640, _T("Main"))
, start_count(0)
, end_count(0)
, counts(0)
, freq(0)
, fps(60) {}

auto APP::Run() -> void
{
    #ifdef _WIN32
    static LARGE_INTEGER hold;
    QueryPerformanceCounter(&hold); start_count = hold.QuadPart;
    QueryPerformanceFrequency(&hold); freq = hold.QuadPart;
    glClearColor(0.2f, 0.21f, 0.22f, 1.0f);
    
    while (Window::WindowsCount() != 0) {
        Window::ProcessMessages();
        auto _hdc = m_Window.opengl()->DrawContext();
        glViewport(0, 0, m_Window.Width(), m_Window.Height());
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Update(1/static_cast<float>(fps));

        SwapBuffers(_hdc);
        Sleep(1);
        QueryPerformanceCounter(&hold); end_count = hold.QuadPart;
        counts = end_count - start_count;
        start_count = end_count;
        fps = freq / counts;
    }
    #endif
}
