#include <core/APP.hpp>
#include <core/Window.hpp>

#if defined(WINDOWS_PLT)
#include <windows.h>
#elif defined(LINUX_PLT)
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <sys/time.h>
#elif defined(WEB_PLT)
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#endif
constexpr auto Wname = "Main";

APP::APP()
    : m_Window(1180, 640, Wname)
    , fps(60.0f) 
{ 
    m_Window.Show();
}


auto APP::Frame() -> void
{
    CWindow::ProcessMessages(&m_Window);
    gl::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    Update(1.0f/fps);
    m_Window.SwapBuffers();
    m_Window.m_Keyboard->UpdatePrevState();
}

auto APP::Run() -> void
{
    gl::ClearColor(0.2f, 0.21f, 0.22f, 1.0f);
    gl::Viewport(0, 0, m_Window.Width(), m_Window.Height());

    #if defined(WINDOWS_PLT) || defined(LINUX_PLT)
    while (!CWindow::WindowShouldClose()) {
        Frame();
    }
    #elif defined(WEB_PLT)
    emscripten_set_main_loop_arg([](void* userData){
        static_cast<APP*>(userData)->Frame();
    }, nullptr, 0, 1);
    #endif
}