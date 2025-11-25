
#include <memory>
#include <random>
#include <Engine.hpp>
#include <game_export.h>
#include <cmrc/cmrc.hpp>

CMRC_DECLARE(res);
inline auto fs = cmrc::res::get_filesystem();

using namespace std;


class Game
{
private:
    class APP& app;
    std::shared_ptr<Shader> vert, frag;
    std::shared_ptr<ShaderProgram> CubeProgram;

    Scene MainScene;
public:
    Game(class APP& app)
        : app(app)
        , vert(std::make_shared<Shader>(cmrc::res::get_filesystem().open("res/Shaders/cube.vert"), GL_VERTEX_SHADER))
        , frag(std::make_shared<Shader>(cmrc::res::get_filesystem().open("res/Shaders/cube.frag"), GL_FRAGMENT_SHADER))
        , CubeProgram(std::make_shared<ShaderProgram>(vert, frag))
        , MainScene()
    {

        ResManager["brik.jpg"]          = Texture2D(cmrc::res::get_filesystem().open("res/brik.jpg"));
        ResManager["brik.png"]          = Texture2D(cmrc::res::get_filesystem().open("res/brik.png"));
        ResManager["kimberley.jpg"]     = Texture2D(cmrc::res::get_filesystem().open("res/kimberley.jpg"));
        ResManager["annie_spratt.jpg"]  = Texture2D(cmrc::res::get_filesystem().open("res/annie_spratt.jpg"));
        ResManager["sand.png"]          = Texture2D(cmrc::res::get_filesystem().open("res/gravelly_sand_diff_4k.png"));

        ResManager["forest.jpg"]= TextureCubeMap(TextureCubeMap::base_to_6facesfiles("res/forest.jpg"));

        ResManager["CubeMattkimberley"]  = Material(ResManager["kimberley.jpg"]);
        ResManager["CubeMattSand"]  = Material(ResManager["sand.png"]);
        ResManager["cubeMesh"]      = Mesh(Mesh::CUBE);
        ResManager["manMesh"]       = Mesh(obj_to_mesh(cmrc::res::get_filesystem().open("res/FinalBaseMesh.obj")));

        constexpr int32_t Grids = 5;

        static thread_local std::mt19937 rng(std::random_device{}());
        static thread_local std::bernoulli_distribution coin(0.5f);

        for(int32_t i = -Grids; i < Grids; i ++){
            for(int32_t j = -Grids; j < Grids; j ++){
                auto m = coin(rng) ? ResManager["CubeMattkimberley"] : ResManager["CubeMattSand"];
                auto meshRes = ResManager["cubeMesh"];
                auto t = Transform({i, 0, j}, {0, 0, 0}, { 0.5f, 0.5f, 0.5f});
            
                MainScene << GameObject(t, m, meshRes);
            }
        }
        MainScene << GameObject(Transform({0, 0, 0}, {0, 0, 0}, { 0.2f, 0.2f, 0.2f}), ResManager["CubeMattSand"], ResManager["manMesh"]);
        MainScene << GameObject(Transform({2, 0, 0}, {0, 0, 0}, { 0.2f, 0.2f, 0.2f}), ResManager["CubeMattkimberley"], ResManager["manMesh"]);

        MainScene.set_skybox(ResManager["forest.jpg"]);
        debug::print("Window title: {}", app.Window.get_title());

        utils::async_repeat_every(1000,
            [&app](){ app.Window.set_title(std::format("{}, {} : {}", sys::host::name_str(),sys::host::arch_str(), app.fps())); }
        );

        debug::print("{} || {}", VecWrapper{gl::OPENGL_FUNCTIONS_NAME}, Type<decltype(gl::OPENGL_FUNCTIONS_NAME)>());
    }

    auto update(float delta) -> void
    {
        camera_mouvment(delta);

        app.render(MainScene, CubeProgram);
    }

    auto on_deltamouse(float dx, float dy) -> void
    {
        MainScene.main_camera().process_mouse_movement(dx, -dy);
    }

    auto camera_mouvment(float delta) -> void
    {
        float speed = app.Keyboard.is_down(Key::LeftShift)? 10.0f : 5.0f;

        auto Hori = app.Keyboard.is_down(Key::W) ? 1.0f : app.Keyboard.is_down(Key::S) ? -1.0f : 0.0f;
        auto Vert = app.Keyboard.is_down(Key::D) ? 1.0f : app.Keyboard.is_down(Key::A) ? -1.0f : 0.0f;
        auto Up   = app.Keyboard.is_down(Key::M) ? 1.0f : app.Keyboard.is_down(Key::N) ? -1.0f : 0.0f;

        auto by = speed * delta;

        MainScene.main_camera().move({ Vert * by, Up * by, Hori * by });
    }
};




// no toche code
extern "C" GAME_API auto new_game(class APP& app) -> void*
{
    return new Game(app);
}

extern "C" GAME_API auto delete_game(void* game) -> void
{
    delete static_cast<Game*>(game);
}

extern "C" GAME_API auto game_update(void* game, float delta) -> void
{
    static_cast<Game*>(game)->update(delta);
}

extern "C" GAME_API auto game_on_deltamouse(void* game, float dx, float dy) -> void
{
    static_cast<Game*>(game)->on_deltamouse(dx, dy);
}

extern "C" GAME_API auto game_link(void** gl_state) -> void
{
    gl::import_opengl_state(gl_state);
}