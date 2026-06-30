#include <chrono>
#include <variant>

#include "APP.hpp"
#include "Window.hpp"
#include "OpenGL.hpp"
#include "Scene.hpp"
#include "ShaderProgram.hpp"
#include "OpenGLRenderer.hpp"

#include <core/SysInfo.hpp>
#include <core/Log.hpp>
#include <core/Event.hpp>

#include <inputs/Keyboard.hpp>
#include <inputs/Mouse.hpp>

constexpr auto WINDOW_WIDTH = 1180;
constexpr auto WINDOW_HEIGHT = 640;

IGame::IGame() : app(APP::self(this)){}
IGame::~IGame() = default;
auto IGame::update(float dt) -> void { logg::trace("update(delta: {})", dt); }
auto IGame::on_deltamouse(float dx, float dy) -> void { logg::trace("on_deltamouse(dx: {}, dy:{})", dx, dy); }

APP::APP()
    : m_Running(true)
    , m_Fps(0.0f)
    , Window(WINDOW_WIDTH, WINDOW_HEIGHT, "")
    , Keyboard()
    , Mouse()
    , m_GApi(Window)
    , Renderer(std::make_unique<OpenGLRenderer>(m_GApi))
    , UiText(m_GApi)
    , Game()
{
    Window.show();
    Window.set_vsync(false);
}

auto APP::self(IGame* g) -> APP&
{
    static APP ins;
    if(g) ins.Game = g;
    return ins;
}

auto APP::frame() -> void
{
    static auto lastTime = std::chrono::steady_clock::now();
    auto now = std::chrono::steady_clock::now();
    float dt = std::chrono::duration<float>(now - lastTime).count();
    lastTime = now;

    float game_dt = (dt > 0.1f) ? 0.1f : dt; // cap

    Game->update(game_dt);

    static float accumulated = 0.0f;
    static int   frameCount = 0;
    ++frameCount;
    accumulated += dt;
    if (accumulated >= 1.0f) {
        m_Fps = static_cast<float>(frameCount) / accumulated;
        accumulated = 0.0f;
        frameCount = 0;
    }

    Renderer->render(Game->Scene);
    UiText.render();
    Window.swap_buffers();
    Keyboard.save_prev_state();
    Mouse.save_prev_state();
}

template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };

auto APP::loop_body(void* ctx) -> void
{
    auto app = static_cast<APP*>(ctx);
    app->Window.poll_events();

    Event event;
    while (EventQ::self().pull(event)) {
        std::visit( overloaded {
            [&app](const CWindow::QuitEvent&) {

                auto ret = app->Window.message_box("Exit", "Are You Sure !");
                if(ret) {
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
                app->Game->on_deltamouse(dx, dy);                
            },
            [](const auto& e) {
                logg::warn("Unhandeled Event: {}", typeid(e).name()); 
            }
        }, event
        );
    }


    //TODO move to the visit
    // normal Mode
    if (app->Keyboard.is_pressed(Key::Space)) {        
        app->Renderer->set_mode(DrawMode::Triangles);
    }

    // Wireframe Mode
    if (app->Keyboard.is_pressed(Key::H)) {        
        app->Renderer->set_mode(DrawMode::Line);
    }

    // Points Mode
    if (app->Keyboard.is_pressed(Key::P)) {        
        app->Renderer->set_mode(DrawMode::Point);
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

auto APP::render_stats() const -> RenderStats
{
    return Renderer->render_stats();
}