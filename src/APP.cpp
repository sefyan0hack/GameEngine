#include <core/APP.hpp>
#include <core/Window.hpp>
#include <core/Event.hpp>
#include <core/Utils.hpp>

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
    , m_Keyboard()
    , m_Mouse()
    , m_LastFrameTime(std::chrono::steady_clock::now())
    , m_SmoothedFPS(60.0f)
{ 
    m_Window.Show();
}


auto APP::Frame(float deltaTime) -> void
{
    gl::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    Update(deltaTime);
    m_Window.SwapBuffers();
    // m_Keyboard.UpdatePrevState();
}

auto APP::LoopBody(void* ctx) -> void
{
    auto app = static_cast<APP*>(ctx);
    auto& window = app->m_Window;

    CWindow::ProcessMessages(&window);

    Event event;
    while (window.PollEvent(event)) {
        std::visit(overloaded {
            [&window](const QuitEvent&) {
                #if defined(WINDOWS_PLT)
                int32_t ret = MessageBoxA(window.Handle(), "Close.", "Exit", MB_YESNO | MB_ICONWARNING);
                if (ret == IDYES)
                #elif defined(LINUX_PLT)
                #endif
                {
                    window.Close();
                }
            },
            [&window](const WindowResizeEvent& e) {
                window.m_Width  = e.width;
                window.m_Height = e.height;
                gl::Viewport(0, 0, e.width, e.height);
            },
            [&app](const Keyboard::Event& e) {
                switch (e.type)
                {
                case Keyboard::Event::Type::Press:
				    app->m_Keyboard.OnKeyPressed(e.key);
                    break;
                case Keyboard::Event::Type::Release:
				    app->m_Keyboard.OnKeyReleased(e.key);
                    break;
                case Keyboard::Event::Type::Repeat:
                    app->m_Keyboard.OnKeyRepeat(e.key);
                    break;
                default:
                    std::unreachable();
                }
            },
            [&app](const Mouse::Event& e) {
                switch (e.type)
                {
                    case Mouse::Event::Type::LPress:
                        app->m_Mouse.OnLeftPressed();
                        break;
                    case Mouse::Event::Type::LRelease:
                        app->m_Mouse.OnLeftReleased();
                        break;
                    case Mouse::Event::Type::RPress:
                        app->m_Mouse.OnRightPressed();
                        break;
                    case Mouse::Event::Type::RRelease:
                        app->m_Mouse.OnRightReleased();
                        break;
                    case Mouse::Event::Type::Move:
                        app->m_Mouse.OnMouseMove(e.x, e.y);
                        break;
                    case Mouse::Event::Type::Enter:
                        app->m_Mouse.OnMouseEnter();
                        break;
                    case Mouse::Event::Type::Leave:
                        app->m_Mouse.OnMouseLeave();
                        break;
                    default:
                    std::unreachable();
                }
            },
            [&app](const MouseWheelEvent& e) {
                app->m_Mouse.OnWheelDelta(e.delta);
            },
            [&app](const MouseRawEvent& e) {
	    		app->m_Mouse.OnRawDelta( e.dx, e.dy );
            },
            [&app](const LoseFocusEvent&) {
		        app->m_Keyboard.ClearState();
            },
            [](const auto&) { /* Unhandeled Events */ },
        }, event);
    }

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