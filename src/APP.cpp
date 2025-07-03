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
    , m_LastFrameTime(std::chrono::steady_clock::now())
    , m_SmoothedFPS(60.0f)
{ 
    m_Window.Show();
}


auto APP::Frame(float deltaTime) -> void
{
    CWindow::ProcessMessages(&m_Window);
    gl::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    Update(deltaTime);
    m_Window.SwapBuffers();
    m_Window.m_Keyboard->UpdatePrevState();
}

auto APP::LoopBody(void* ctx) -> void
{
    auto app = static_cast<APP*>(ctx);

    auto now = std::chrono::steady_clock::now();
    float deltaTime = std::chrono::duration<float>(now - app->m_LastFrameTime).count();
    app->m_LastFrameTime = now;

    app->m_Fps = 1.0f / deltaTime;
    app->m_SmoothedFPS = 0.9f * app->m_SmoothedFPS + 0.1f * app->m_Fps;

    app->Frame(deltaTime);
    //todo: Frame Pacing
}

auto APP::Run() -> void
{
    gl::ClearColor(0.2f, 0.21f, 0.22f, 1.0f);
    gl::Viewport(0, 0, m_Window.Width(), m_Window.Height());

    #if defined(WINDOWS_PLT) || defined(LINUX_PLT)
    while (!CWindow::WindowShouldClose()) {
        LoopBody(this);
    }
    #elif defined(WEB_PLT)
    emscripten_set_main_loop_arg(LoopBody, this, 0, 1);
    #endif
}

auto APP::Fps() const -> float
{
    return m_Fps;
}
auto APP::SmoothedFPS() const -> float
{
    return m_SmoothedFPS;
}

auto APP::DeltaTime() const -> float
{
    return 1.0f/m_Fps;
}