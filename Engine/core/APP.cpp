#include "APP.hpp"
#include "Window.hpp"
#include "Event.hpp"
#include "Utils.hpp"
#include "OpenGL.hpp"
#include "Scene.hpp"

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
    Window.Show();
}

APP::~APP()
{
    ClearEvents();
}

auto APP::Frame(float deltaTime) -> void
{
    gl::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    Update(deltaTime);
    Window.SwapBuffers();
    Keyboard.SavePrevState();
    Mouse.SavePrevState();
}

auto APP::Render(const Scene& scene, std::shared_ptr<ShaderProgram> program) -> void
{
    m_Renderer.Render(scene, program);
}

auto APP::LoopBody(void* ctx) -> void
{
    auto app = static_cast<APP*>(ctx);
    auto& window = app->Window;

    CWindow::ProcessMessages(&window);

    Event event;
    while (app->PollEvent(event)) {
        std::visit(overloaded {
            [&app](const CWindow::QuitEvent&) {
                #if defined(WINDOWS_PLT)
                int32_t ret = MessageBoxA(app->Window.Handle(), "Close.", "Exit", MB_YESNO | MB_ICONWARNING);
                if (ret == IDYES)
                #elif defined(LINUX_PLT)
                #endif
                {
                    app->Window.Close();
                    app->m_Running = false;
                }
            },
            [&app](const CWindow::ResizeEvent& e) {
                app->Window.m_Width = e.width;
                app->Window.m_Height = e.height;
                app->m_Renderer.SetViewPort(0, 0, e.width, e.height);
            },
            [&app](const CWindow::LoseFocusEvent&) {
		        app->Keyboard.ClearState();
		        app->Mouse.ClearState();
		        app->ApplicationEventQueue.clear();
            },
            [&app](const Keyboard::KeyDownEvent& e) {
				app->Keyboard.OnKeyDown(e.key);
            },
            [&app](const Keyboard::KeyUpEvent& e) {
				app->Keyboard.OnKeyUp(e.key);
            },
            [&app](const Mouse::ButtonDownEvent& e) {
                app->Mouse.OnButtonDown(e.btn);
            },
            [&app](const Mouse::ButtonUpEvent& e) {
                app->Mouse.OnButtonUp(e.btn);
            },
            [&app](const Mouse::EnterEvent&) {
                app->Mouse.OnMouseEnter();
            },
            [&app](const Mouse::LeaveEvent&) {
                app->Mouse.OnMouseLeave();
            },
            [&app](const Mouse::MoveEvent& e) {
                app->Mouse.OnMouseMove(e.x, e.y);
            },
            [&app](const Mouse::MovementEvent& e) {
                app->Mouse.OnRawDelta(e.dx, e.dy);
                auto [dx, dy] = app->Mouse.GetRawDelta();
                app->ViewCamera.ProcessMouseMovement(dx, -dy);
            },
            [](const auto& e) {
                if( ::type_name<decltype(e)>() == "const std::monostate&") throw CException(" nnnnnn ");
                Debug::Print("Unhandeled Event: {}", ::type_name<decltype(e)>()); 
            },
        }, event);
    }

    auto now = std::chrono::steady_clock::now();
    float deltaTime = std::chrono::duration<float>(now - app->m_LastFrameTime).count();
    app->m_LastFrameTime = now;

    app->m_Fps = 1.0f / deltaTime;
    app->m_SmoothedFPS = 0.9f * app->m_SmoothedFPS + 0.1f * app->m_Fps;

    // Wireframe Mode
    if (app->Keyboard.IsPressed(Key::H)) {
        static bool flip = false;
    
        #if defined(WEB_PLT)
        static bool webPolyModeChecked = false;
        static bool webPolyModeAvailable = false;
        if (!webPolyModeChecked) {
            webPolyModeAvailable = app->m_Renderer.opengl().HasExtension("WEBGL_polygon_mode");
            webPolyModeChecked = true;
        }
        #endif
    
        if (!flip) {
            #if defined(WEB_PLT)
            if (webPolyModeAvailable) {
                EM_ASM({
                    const gl = Module.ctx;
                    const ext = gl.getExtension("WEBGL_polygon_mode");
                    ext.polygonModeWEBGL(gl.FRONT_AND_BACK, ext.LINE_WEBGL);
                });
            }
            #else
            gl::Enable(GL_LINE_SMOOTH);
            gl::Enable(GL_POLYGON_OFFSET_LINE);
            gl::PolygonOffset(-1.0f, -1.0f);
            gl::PolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            #endif
            flip = true;
        } else {
            #if defined(WEB_PLT)
            if (webPolyModeAvailable) {
                EM_ASM({
                    const gl = Module.ctx;
                    const ext = gl.getExtension("WEBGL_polygon_mode");
                    ext.polygonModeWEBGL(gl.FRONT_AND_BACK, ext.FILL_WEBGL);
                });
            }
            #else
            gl::Disable(GL_POLYGON_OFFSET_LINE);
            gl::Disable(GL_LINE_SMOOTH);
            gl::PolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            #endif
            flip = false;
        }
    }

    #if !defined(WEB_PLT)
        // Points Mode
        if (app->Keyboard.IsPressed(Key::P)){
            static bool flip = false;
            if(flip == false){
                flip = !flip;
                gl::Enable(GL_PROGRAM_POINT_SIZE);
                GLfloat widths[2];
                auto& min = widths[0];
                auto& max = widths[1];
                gl::GetFloatv(GL_ALIASED_LINE_WIDTH_RANGE, widths);
                gl::PointSize((max-min)/2);

                gl::PolygonMode(GL_FRONT_AND_BACK, GL_POINT);
            }else{
                flip = !flip;
                gl::Disable(GL_PROGRAM_POINT_SIZE);
                gl::PolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
        }
    #endif

    // Fullscreen 
    if(app->Keyboard.IsPressed(Key::F11)){
        app->Window.ToggleFullScreen();
    }

    // Lock Mouse
    if(app->Keyboard.IsPressed(Key::L) ){
        static bool on = false;
        if(!on){
            app->Mouse.Lock(app->Window);
            on = true;
        }else{
            app->Mouse.UnLock();
            on = false;
        }
    }

    app->Frame(deltaTime);
    //todo: Frame Pacing
}

auto APP::Run() -> void
{
    // m_Renderer.SetViewPort(0, 0, Window.Width(), Window.Height());

    #if defined(WINDOWS_PLT) || defined(LINUX_PLT)
    while (m_Running) {
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

bool APP::PollEvent(Event& event) { return ApplicationEventQueue.poll(event); }
void APP::WaitEvent(Event& event) { ApplicationEventQueue.wait_and_poll(event); }
void APP::ClearEvents() { ApplicationEventQueue.clear(); }
auto APP::PushEvent(const Event& event) -> void { ApplicationEventQueue.push(event); }