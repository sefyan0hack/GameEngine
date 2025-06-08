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
// Web platform main loop handler
void APP::WebLoop(void* userData) {
    APP* app = static_cast<APP*>(userData);
    
    // Get current time for delta calculation
    static double lastTime = emscripten_get_now();
    double currentTime = emscripten_get_now();
    float delta = static_cast<float>((currentTime - lastTime)) / 1000.0f;  // Convert to seconds
    lastTime = currentTime;
    
    app->Update(delta);
    app->Render();
    
    // Check if we should stop the loop
    if (CWindow::WindowsCount() == 0) {
        emscripten_cancel_main_loop();
    }
}
#endif


auto APP::Render() -> void {
    gl::Viewport(0, 0, m_Window.Width(), m_Window.Height());
    gl::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
    #if defined(WINDOWS_PLT)
    SwapBuffers(m_Window.opengl()->DrawContext());
    #elif defined(LINUX_PLT)
    glXSwapBuffers(m_Window.DrawContext(), m_Window.WindowHandle());
    #elif defined(WEB_PLT)
    // No buffer swap needed - handled automatically by browser
    #endif
    
    #if defined(WEB_PLT)
    end_count = static_cast<int64_t>(emscripten_get_now());
    counts = static_cast<int64_t>(end_count - start_count);
    start_count = end_count;
    if (counts > 0) {
        fps = static_cast<int64_t>(1000.0 / counts);
    }
    #endif
}

auto APP::Run() -> void
{
    gl::ClearColor(0.2f, 0.21f, 0.22f, 1.0f);

    #if defined(WINDOWS_PLT)
    QueryPerformanceCounter((LARGE_INTEGER *)&start_count);
    QueryPerformanceFrequency((LARGE_INTEGER *)&freq);
    
    while (CWindow::WindowsCount() != 0) {
        CWindow::ProcessMessages();
        auto _hdc = m_Window.opengl()->DrawContext();
        gl::Viewport(0, 0, m_Window.Width(), m_Window.Height());
        gl::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Update(1/static_cast<float>(fps));

        SwapBuffers(_hdc);
        Sleep(1);
        QueryPerformanceCounter((LARGE_INTEGER *)&end_count);
        counts = end_count - start_count;
        start_count = end_count;
        fps = freq / counts;
    }
    #elif defined(LINUX_PLT)
    Display* display = m_Window.DrawContext();
    Window xid = m_Window.WindowHandle();
    Atom wmDeleteMessage = XInternAtom(display, "WM_DELETE_WINDOW", false);
    XSetWMProtocols(display, xid, &wmDeleteMessage, 1);

    struct timespec start_time, end_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);
        
    while (CWindow::WindowsCount() != 0) {
        while (XPending(display)) {
            XEvent event;
            XNextEvent(display, &event);
            
            switch (event.type) {
                case Expose:
                    // Handle window expose event
                    break;
                    
                case ConfigureNotify:
                    // Handle window resize
                    // m_Window.SetDims(event.xconfigure.width, event.xconfigure.height);
                    break;
                    
                case KeyPress:
                    // Handle keyboard input
                    break;
                    
                case ClientMessage:
                    if (event.xclient.data.l[0] == wmDeleteMessage)
                        // m_Window.Destroy();
                    break;
            }
        }

        // Rendering
        gl::Viewport(0, 0, m_Window.Width(), m_Window.Height());
        gl::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Update(1/static_cast<float>(fps));

        // Swap buffers
        glXSwapBuffers(display, xid);

        // Frame timing
        clock_gettime(CLOCK_MONOTONIC, &end_time);
        long ns_diff = (end_time.tv_sec - start_time.tv_sec) * 1000000000L 
                     + (end_time.tv_nsec - start_time.tv_nsec);
        float delta = ns_diff / 1000000000.0f;
        
        // Maintain 60 FPS
        if (delta < 1.0f/fps) {
            timespec sleep_time{
                0,
                static_cast<long>((1.0f/fps - delta) * 1000000000L)
            };
            nanosleep(&sleep_time, nullptr);
        }
        
        clock_gettime(CLOCK_MONOTONIC, &start_time);
    }
    #elif defined(WEB_PLT)
    start_count = emscripten_get_now();
    emscripten_set_main_loop_arg(WebLoop, this, 0, 1);
    #endif
}
