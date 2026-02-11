#include <chrono>

#include "APP.hpp"
#include "Utils.hpp"
#include "SysInfo.hpp"
#include "Log.hpp"
#include "Event.hpp"

#include <inputs/Keyboard.hpp>
#include <inputs/Mouse.hpp>
#include <graphics/Window.hpp>
#include <graphics/OpenGL.hpp>
#include <graphics/Scene.hpp>
#include <graphics/ShaderProgram.hpp>
#include <graphics/OpenGLRenderer.hpp>

extern "C" {
    extern auto game_ctor() -> void*;
    extern auto game_dtor(void*) -> void;
    extern auto game_set_app(APP*) -> void;
    extern auto game_update(void*, float) -> void;
    extern auto game_on_deltamouse(void*, float, float) -> void;
}

[[maybe_unused]] constexpr auto WINDOW_WIDTH = 1180;
[[maybe_unused]] constexpr auto WINDOW_HIEGHT = 640;

APP::APP()
    : m_Running(true)
    , m_Fps(60.0f)
    , m_FrameCount(0)
    , m_AccumulatedTime(0.0f)
    , Window(WINDOW_WIDTH, WINDOW_HIEGHT, "")
    , Keyboard()
    , Mouse()
    , m_GApi(Window)
    , Renderer(new OpenGLRenderer(m_GApi))
    , UiText(m_GApi)
    , MainScene()
    , Game()
{
    game_set_app(this);
    Game = game_ctor();

    Window.show();
    Window.set_vsync(true);
}

APP::~APP()
{
    if(Game) game_dtor(Game);
    if(Renderer) delete Renderer;
}

auto APP::frame() -> void
{
    auto begin = std::chrono::steady_clock::now();

    Renderer->clear_screen(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    game_update(Game, 1.0f/m_Fps);
    Renderer->render(MainScene);
    UiText.render();
    Window.swap_buffers();
    Keyboard.save_prev_state();
    Mouse.save_prev_state();

    auto elapsed = std::chrono::steady_clock::now() - begin;
    float deltaTime = std::chrono::duration<float>(elapsed).count();

    m_AccumulatedTime += deltaTime;
    ++m_FrameCount;

    if (m_AccumulatedTime >= 1.0f) {
        m_Fps = static_cast<float>(m_FrameCount) / m_AccumulatedTime; // Average FPS over the last second
        m_AccumulatedTime = 0.0f;
        m_FrameCount = 0;
    }
}

auto APP::loop_body(void* ctx) -> void
{
    auto app = static_cast<APP*>(ctx);
    app->Window.process_messages();

    Event event;
    while (EventQ::self().pull(event)) {
        utils::match(event,
            [&app](const CWindow::QuitEvent&) {

                auto ret = app->Window.message_box("Exit", "Are You Sure !");
                if(ret) {
                    app->Window.close();
                    app->m_Running = false;
                }
            },
            [&app](const CWindow::ResizeEvent& e) {
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
                app->Mouse.button_down(e.btn);
            },
            [&app](const Mouse::ButtonUpEvent& e) {
                app->Mouse.button_up(e.btn);
            },
            [&app](const Mouse::EnterEvent&) {
                app->Mouse.mouse_entered();
            },
            [&app](const Mouse::LeaveEvent&) {
                app->Mouse.mouse_leaved();
            },
            [&app](const Mouse::MoveEvent& e) {
                app->Mouse.mouse_moved(e.x, e.y);
            },
            [&app](const Mouse::MovementEvent& e) {
                app->Mouse.rawdelta(e.dx, e.dy);
                auto [dx, dy] = app->Mouse.get_rawdelta();
                game_on_deltamouse(app->Game, dx, dy);
            },
            [](const auto& e) {
                debug::log("Unhandeled Event: {}", typeid(e).name()); 
            }
        );
    }

    // normal Mode
    if (app->Keyboard.is_pressed(Key::Space)) {        
        app->Renderer->normal_mode();
    }

    // Wireframe Mode
    if (app->Keyboard.is_pressed(Key::H)) {        
        app->Renderer->wireframe_mode();
    }

    // Points Mode
    if (app->Keyboard.is_pressed(Key::P)) {        
        app->Renderer->points_mode();
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

    app->frame();
}

auto APP::fps() const -> float
{
    return m_Fps;
}

auto APP::deltatime() const -> float
{
    return 1.0f/m_Fps;
}