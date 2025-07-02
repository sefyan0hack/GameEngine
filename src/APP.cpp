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
    , fps(60.0) 
{ 
    m_Window.Show();
}


#if defined(WEB_PLT)
void APP::WebLoop(void* userData) {
    APP* app = static_cast<APP*>(userData);

    gl::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    app->Update(1.0/fps);
    
    if (CWindow::WindowShouldClose()) {
        emscripten_cancel_main_loop();
    }
    app->m_Window.m_Keyboard->UpdatePrevState();

}
#endif

auto APP::Run() -> void
{
    [[maybe_unused]] auto windowHandle = m_Window.Handle();
    [[maybe_unused]] auto DrawCtx = m_Window.DrawContext();
    [[maybe_unused]] auto OpenglCtx = m_Window.opengl()->DrawContext();

    gl::ClearColor(0.2f, 0.21f, 0.22f, 1.0f);
    gl::Viewport(0, 0, m_Window.Width(), m_Window.Height());

    #if defined(WINDOWS_PLT) || defined(LINUX_PLT)

    while (!CWindow::WindowShouldClose()) {
        CWindow::ProcessMessages(&m_Window);
        // Rendering
        gl::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        Update(1.0/fps);

        // Swap buffers
        #if defined(WINDOWS_PLT)
        SwapBuffers(OpenglCtx);
        #elif defined(LINUX_PLT)
        glXSwapBuffers(DrawCtx, windowHandle);
        #endif

        m_Window.m_Keyboard->UpdatePrevState();
    }
    #elif defined(WEB_PLT)
    emscripten_set_main_loop_arg(WebLoop, this, fps, 1);
    #endif
}