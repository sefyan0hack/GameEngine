#include <input/Keyboard.hpp>
#include <input/Mouse.hpp>
#include <graphics/Window.hpp>
#include <graphics/OpenGL.hpp>
#include <graphics/Scene.hpp>
#include <graphics/ShaderProgram.hpp>
#include <graphics/OpenGLRenderer.hpp>

#include "APP.hpp"
#include "Utils.hpp"
#include "DynLib.hpp"
#include "SysInfo.hpp"
#include "Log.hpp"
#include "Event.hpp"

#if defined(WINDOWS_PLT)
#include <windows.h>
#undef near
#undef far
#elif defined(LINUX_PLT)
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#elif defined(WEB_PLT)
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#endif

#undef X
#define X(name, r, args) extern "C" DECLARE_FUNCTION(name, r, args);

#if defined(GAME_HOT_RELOADABLE)
    #define GET_GAME_FUNCTION(name) lib.function<APP::name##_t>(#name)
    #define GAME_LIB_NOW true
#else
    #define GET_GAME_FUNCTION(name) reinterpret_cast<APP::name##_t>(::name)
    #define GAME_LIB_NOW false
    GAME_API
#endif


constexpr auto WINDOW_WIDTH = 1180;
constexpr auto WINDOW_HIEGHT = 640;

#undef X
#define X(name, r, args) , name(+[] args -> r { throw Exception(" `{} {}` is null ", #r, #name #args ); })

APP::APP()
    : Window(WINDOW_WIDTH, WINDOW_HIEGHT, "")
    , Keyboard()
    , Mouse()
    , m_Running(true)
    , m_LastFrameTime(std::chrono::steady_clock::now())
    , m_Fps(60.0f)
    , Renderer(new OpenGLRenderer(Window))
    , MainScene()
    , lib("Game", GAME_LIB_NOW)
    , Game()
    GAME_API
{
    init_game_functions();

    game_link(gl::export_opengl_functions());
    game_set_app(this);
    Game = game_ctor();

    Window.show();
    Window.set_vsync(true);
}

APP::~APP()
{
    if(Renderer) delete Renderer;
}

auto APP::init_game_functions() -> void
{
    #undef X
    #define X(name, r, args) name = GET_GAME_FUNCTION(name);
    GAME_API
}


auto APP::hot_reload_game_library() -> bool
{
    try {
        game_dtor(Game);

        lib.unload();
        lib.load();

        init_game_functions();

        game_link(gl::export_opengl_functions());
        game_set_app(this);
        Game = game_ctor();

        debug::log("Game library hot-reloaded successfully");
        return true;

    } catch (const std::exception& e) {
        debug::log("Hot reload failed: {}", e.what());
        return false;
    }
}

auto APP::frame(float deltaTime) -> void
{
    Renderer->clear_screen(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    game_update(Game, deltaTime);
    Renderer->render(MainScene, ShaderProgram::default_program());
    Window.swap_buffers();
    Keyboard.save_prev_state();
    Mouse.save_prev_state();
}

auto APP::loop_body(void* ctx) -> void
{
    auto app = static_cast<APP*>(ctx);
    app->Window.process_messages();

    Event event;
    while (EventQ::self().pull(event)) {
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
                app->Window.resize(e.width, e.height);
                app->Renderer->set_viewport(0, 0, e.width, e.height);
            },
            [&app](const CWindow::LoseFocusEvent&) {
		        app->Keyboard.clear_state();
		        app->Mouse.clear_state();
		        EventQ::self().clear();
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
                app->game_on_deltamouse(app->Game, dx, dy);
            },
            [](const auto& e) {
                if( ::type_name<decltype(e)>() == "const std::monostate&") throw Exception(" nnnnnn ");
                debug::log("Unhandeled Event: {}", ::type_name<decltype(e)>()); 
            }
        );
    }

    auto now = std::chrono::steady_clock::now();
    float deltaTime = std::chrono::duration<float>(now - app->m_LastFrameTime).count();
    app->m_LastFrameTime = now;

    app->m_Fps = 1.0f / deltaTime;

    // Wireframe Mode
    static bool wireframe_enabled = false;
    static bool h_key_was_pressed = false;

    if (app->Keyboard.is_pressed(Key::H)) {        
        if (!h_key_was_pressed) {
            wireframe_enabled = !wireframe_enabled;
            
            if (wireframe_enabled) {
                app->Renderer->enable_wireframe();
            } else {
                app->Renderer->disable_wireframe();
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
                app->Renderer->enable_points();
            } else {
                app->Renderer->disable_points();
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

    #if defined(GAME_HOT_RELOADABLE)
    static bool r_key_was_pressed = false;
    if (app->Keyboard.is_pressed(Key::R)) {
        if (!r_key_was_pressed) {
            app->hot_reload_game_library();
            r_key_was_pressed = true;
        }
    } else {
        r_key_was_pressed = false;
    }

    #endif
    app->frame(deltaTime);
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

auto APP::deltatime() const -> float
{
    return 1.0f/m_Fps;
}