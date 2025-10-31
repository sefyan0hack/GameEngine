#include "APP.hpp"
#include "Window.hpp"
#include "Event.hpp"
#include "Utils.hpp"
#include <graphics/OpenGL.hpp>
#include <graphics/Scene.hpp>

#if defined(WINDOWS_PLT)
#include <windows.h>
#undef near
#undef far
#elif defined(LINUX_PLT)
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <sys/time.h>
#elif defined(WEB_PLT)
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#endif

constexpr auto Wname = "Game";
constexpr auto WINDOW_WIDTH = 1180;
constexpr auto WINDOW_HIEGHT = 640;

APP::APP()
    : Window(WINDOW_WIDTH, WINDOW_HIEGHT, Wname, ApplicationEventQueue)
    , ViewCamera()
    , Keyboard()
    , Mouse()
    , m_Renderer(Window, ViewCamera)
    , m_Running(true)
    , m_LastFrameTime(std::chrono::steady_clock::now())
    , m_SmoothedFPS(60.0f)
{ 
    Window.show();
    Window.set_vsync(true);
}

APP::~APP()
{
    clear_events();
}

auto APP::frame(float deltaTime) -> void
{
    m_Renderer.clear_screen(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    update(deltaTime);
    Window.swap_buffers();
    Keyboard.save_prev_state();
    Mouse.save_prev_state();
}

auto APP::render(const Scene& scene, std::shared_ptr<ShaderProgram> program) -> void
{
    m_Renderer.render(scene, program);
}

auto APP::loop_body(void* ctx) -> void
{
    auto app = static_cast<APP*>(ctx);
    auto& window = app->Window;

    CWindow::process_messages(&window);

    Event event;
    while (app->pull_event(event)) {
        utils::match(event,
            [&app](const CWindow::QuitEvent&) {
                #if defined(WINDOWS_PLT)
                int32_t ret = MessageBoxA(app->Window.handle(), "Close.", "Exit", MB_YESNO | MB_ICONWARNING);
                if (ret == IDYES)
                #elif defined(LINUX_PLT)
                #endif
                {
                    app->Window.close();
                    app->m_Running = false;
                }
            },
            [&app](const CWindow::ResizeEvent& e) {
                app->Window.m_Width = e.width;
                app->Window.m_Height = e.height;
                app->m_Renderer.set_viewport(0, 0, e.width, e.height);
            },
            [&app](const CWindow::LoseFocusEvent&) {
		        app->Keyboard.clear_state();
		        app->Mouse.clear_state();
		        app->ApplicationEventQueue.clear();
            },
            [&app](const Keyboard::KeyDownEvent& e) {
				app->Keyboard.on_key_down(e.key);
            },
            [&app](const Keyboard::KeyUpEvent& e) {
				app->Keyboard.on_key_up(e.key);
            },
            [&app](const Mouse::ButtonDownEvent& e) {
                app->Mouse.on_button_down(e.btn);
            },
            [&app](const Mouse::ButtonUpEvent& e) {
                app->Mouse.on_button_up(e.btn);
            },
            [&app](const Mouse::EnterEvent&) {
                app->Mouse.on_mouse_enter();
            },
            [&app](const Mouse::LeaveEvent&) {
                app->Mouse.on_mouse_leave();
            },
            [&app](const Mouse::MoveEvent& e) {
                app->Mouse.on_mouse_move(e.x, e.y);
            },
            [&app](const Mouse::MovementEvent& e) {
                app->Mouse.on_rawdelta(e.dx, e.dy);
                auto [dx, dy] = app->Mouse.get_rawdelta();
                app->ViewCamera.process_mouse_movement(dx, -dy);
            },
            [](const auto& e) {
                if( ::type_name<decltype(e)>() == "const std::monostate&") throw Exception(" nnnnnn ");
                debug::print("Unhandeled Event: {}", ::type_name<decltype(e)>()); 
            }
        );
    }

    auto now = std::chrono::steady_clock::now();
    float deltaTime = std::chrono::duration<float>(now - app->m_LastFrameTime).count();
    app->m_LastFrameTime = now;

    app->m_Fps = 1.0f / deltaTime;
    app->m_SmoothedFPS = 0.9f * app->m_SmoothedFPS + 0.1f * app->m_Fps;

    // Wireframe Mode
    static bool wireframe_enabled = false;
    static bool h_key_was_pressed = false;

    if (app->Keyboard.is_pressed(Key::H)) {        
        if (!h_key_was_pressed) {
            wireframe_enabled = !wireframe_enabled;
            
            if (wireframe_enabled) {
                app->m_Renderer.enable_wireframe();
            } else {
                app->m_Renderer.disable_wireframe();
            }
            
            h_key_was_pressed = true;
        }
    } else {
        h_key_was_pressed = false;
    }

    // Points Mode
    static bool points_enabled = false;
    static bool p_key_was_pressed = false;

    if (app->Keyboard.is_pressed(Key::P)) {        
        if (!p_key_was_pressed) {
            points_enabled = !points_enabled;
            
            if (points_enabled) {
                app->m_Renderer.enable_points();
            } else {
                app->m_Renderer.disable_points();
            }
            
            p_key_was_pressed = true;
        }
    } else {
        p_key_was_pressed = false;
    }

    // Fullscreen 
    if(app->Keyboard.is_pressed(Key::F11)){
        app->Window.toggle_fullscreen();
    }

    // Lock Mouse
    if(app->Keyboard.is_pressed(Key::L) ){
        static bool on = false;
        if(!on){
            app->Mouse.lock(app->Window);
            on = true;
        }else{
            app->Mouse.unlock();
            on = false;
        }
    }

    app->frame(deltaTime);
    //todo: Frame Pacing
}

auto APP::run() -> void
{
    #if defined(WINDOWS_PLT) || defined(LINUX_PLT)
    while (m_Running) {
        loop_body(this);
    }
    #elif defined(WEB_PLT)
    emscripten_set_main_loop_arg(loop_body, this, 0, 1);
    #endif
}

auto APP::fps() const -> float
{
    return m_Fps;
}
auto APP::smooth_fps() const -> float
{
    return m_SmoothedFPS;
}

auto APP::deltatime() const -> float
{
    return 1.0f/m_Fps;
}

auto APP::push_event(const Event& event) -> void { ApplicationEventQueue.push(event); }
bool APP::pull_event(Event& event) { return ApplicationEventQueue.poll(event); }
void APP::wait_event(Event& event) { ApplicationEventQueue.wait_and_poll(event); }
void APP::clear_events() { ApplicationEventQueue.clear(); }