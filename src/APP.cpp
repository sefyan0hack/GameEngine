#include <core/APP.hpp>
#include <core/Window.hpp>

constexpr auto Wname = "Main";

APP::APP()
    : m_Window(1180, 640, Wname)
    , start_count(0)
    , end_count(0)
    , counts(0)
    , freq(0)
    , fps(60) 
{ 
    m_Window.Show();
}

auto APP::Run() -> void
{
    #if defined(WINDOWS_PLT)
    QueryPerformanceCounter((LARGE_INTEGER *)&start_count);
    QueryPerformanceFrequency((LARGE_INTEGER *)&freq);
    glClearColor(0.2f, 0.21f, 0.22f, 1.0f);
    
    while (CWindow::WindowsCount() != 0) {
        CWindow::ProcessMessages();
        auto _hdc = m_Window.opengl()->DrawContext();
        glViewport(0, 0, m_Window.Width(), m_Window.Height());
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Update(1/static_cast<float>(fps));

        SwapBuffers(_hdc);
        Sleep(1);
        QueryPerformanceCounter((LARGE_INTEGER *)&end_count);
        counts = end_count - start_count;
        start_count = end_count;
        fps = freq / counts;
    }
    #endif
}
