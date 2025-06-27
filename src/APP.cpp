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
    , start_count(0)
    , end_count(0)
    , counts(0)
    , freq(0)
    , fps(60) 
{ 
    m_Window.Show();
}


#if defined(WEB_PLT)
void APP::WebLoop(void* userData) {
    APP* app = static_cast<APP*>(userData);
    
    static double lastTime = emscripten_get_now();
    double currentTime = emscripten_get_now();
    float delta = static_cast<float>((currentTime - lastTime)) / 1000.0f;
    lastTime = currentTime;

    gl::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    app->Update(delta);

    app->end_count = static_cast<int64_t>(emscripten_get_now());
    app->counts = static_cast<int64_t>(app->end_count - app->start_count);
    app->start_count = app->end_count;
    if (app->counts > 0) {
        app->fps = static_cast<int64_t>(1000.0 / app->counts);
    }
    
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

    gl::ClearColor(0.2f, 0.21f, 0.22f, 1.0f);
    gl::Viewport(0, 0, m_Window.Width(), m_Window.Height());

    #if defined(WINDOWS_PLT)
    QueryPerformanceCounter((LARGE_INTEGER *)&start_count);
    QueryPerformanceFrequency((LARGE_INTEGER *)&freq);
    
    while (!CWindow::WindowShouldClose()) {
        CWindow::ProcessMessages(&m_Window);
        auto _hdc = m_Window.opengl()->DrawContext();

        gl::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        Update(1/static_cast<float>(fps));

        SwapBuffers(_hdc);
        Sleep(1);
        QueryPerformanceCounter((LARGE_INTEGER *)&end_count);
        counts = end_count - start_count;
        start_count = end_count;
        fps = freq / counts;
        m_Window.m_Keyboard->UpdatePrevState();
    }
    #elif defined(LINUX_PLT)
    struct timespec start_time, end_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    constexpr uint64_t NS_PER_SEC = 1000000000ULL;    // Nanoseconds per second

    while (!CWindow::WindowShouldClose()) {
        CWindow::ProcessMessages(&m_Window);
        // Rendering
        gl::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        Update(1/static_cast<float>(fps));

        // Swap buffers
        glXSwapBuffers(DrawCtx, windowHandle);

        struct timespec sleep_time = {0, 1000000}; // 1ms in ns
        nanosleep(&sleep_time, NULL);

        clock_gettime(CLOCK_MONOTONIC, &end_time);

        auto start_ns = start_time.tv_sec * NS_PER_SEC + start_time.tv_nsec;
        auto end_ns = end_time.tv_sec * NS_PER_SEC + end_time.tv_nsec;
        counts = end_ns - start_ns;
        start_time = end_time;

        fps = (counts > 0) ? (NS_PER_SEC / counts) : 0;

        m_Window.m_Keyboard->UpdatePrevState();
    }
    #elif defined(WEB_PLT)
    start_count = emscripten_get_now();
    emscripten_set_main_loop_arg(WebLoop, this, 0, 1);
    #endif
}
