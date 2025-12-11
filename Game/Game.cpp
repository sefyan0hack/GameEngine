
#include <memory>
#include <random>
#include <Engine.hpp>
#include <game_export.h>

APP* app;
extern cmrc::embedded_filesystem embed_filesystem;

using namespace std;


class GAME_EXPORT Game
{
private:
    std::shared_ptr<Shader> vert, frag;
    std::shared_ptr<ShaderProgram> CubeProgram;

    Scene MainScene;
public:
    Game()
        : vert(std::make_shared<Shader>(embed_filesystem.open("res/Shaders/cube.vert"), GL_VERTEX_SHADER))
        , frag(std::make_shared<Shader>(embed_filesystem.open("res/Shaders/cube.frag"), GL_FRAGMENT_SHADER))
        , CubeProgram(std::make_shared<ShaderProgram>(vert, frag))
        , MainScene()
    {

        ResMan::RES("brik.jpg")          = std::make_shared<Texture2D>(embed_filesystem.open("res/brik.jpg"));
        ResMan::RES("brik.png")          = std::make_shared<Texture2D>(embed_filesystem.open("res/brik.png"));
        ResMan::RES("kimberley.jpg")     = std::make_shared<Texture2D>(embed_filesystem.open("res/kimberley.jpg"));
        ResMan::RES("annie_spratt.jpg")  = std::make_shared<Texture2D>(embed_filesystem.open("res/annie_spratt.jpg"));
        ResMan::RES("sand.png")          = std::make_shared<Texture2D>(embed_filesystem.open("res/gravelly_sand_diff_4k.png"));

        ResMan::RES("forest.jpg")= std::make_shared<TextureCubeMap>(TextureCubeMap::base_to_6facesfiles("res/forest.jpg"));

        ResMan::RES("CubeMattkimberley")  = std::make_shared<Material>(ResMan::RES("kimberley.jpg"));
        ResMan::RES("CubeMattSand")  = std::make_shared<Material>(ResMan::RES("sand.png"));
        ResMan::RES("cubeMesh")      = std::make_shared<Mesh>(Mesh::CUBE);
        ResMan::RES("manMesh")       = std::make_shared<Mesh>(obj_to_mesh(embed_filesystem.open("res/FinalBaseMesh.obj")));

        constexpr int32_t Grids = 5;

        std::mt19937 rng(std::random_device{}());
        std::bernoulli_distribution coin(0.5f);

        for(int32_t i = -Grids; i < Grids; i ++){
            for(int32_t j = -Grids; j < Grids; j ++){
                auto m = coin(rng) ? ResMan::RES("CubeMattkimberley") : ResMan::RES("CubeMattSand");
                auto meshRes = ResMan::RES("cubeMesh");
                auto t = Transform({i, 0, j}, {0, 0, 0}, { 0.5f, 0.5f, 0.5f});
            
                MainScene << GameObject(t, m, meshRes);
            }
        }
        MainScene << GameObject(Transform({0, 0, 0}, {0, 0, 0}, { 0.2f, 0.2f, 0.2f}), ResMan::RES("CubeMattSand"), ResMan::RES("manMesh"));
        MainScene << GameObject(Transform({2, 0, 0}, {0, 0, 0}, { 0.2f, 0.2f, 0.2f}), ResMan::RES("CubeMattkimberley"), ResMan::RES("manMesh"));

        MainScene.set_skybox(ResMan::RES("forest.jpg"));
        debug::print("Window title: {}", app->Window.get_title());

        utils::async_repeat_every(1000,
            [](){ app->Window.set_title(std::format("{}, {} : {}", os::host::name(),os::host::arch(), app->fps())); }
        );

        // debug::print("{} || {}", VecWrapper{gl::OPENGL_FUNCTIONS_NAME}, Type<decltype(gl::OPENGL_FUNCTIONS_NAME)>());
        debug::print("{}", VecWrapper{gl::OPENGL_FUNCTIONS_NAME});
    }

    auto update(float delta) -> void
    {
        camera_mouvment(delta);

        app->render(MainScene, CubeProgram);
    }

    auto on_deltamouse(float dx, float dy) -> void
    {
        MainScene.main_camera().process_mouse_movement(dx, -dy);
    }

    auto camera_mouvment(float delta) -> void
    {
        float speed = app->Keyboard.is_down(Key::LeftShift)? 10.0f : 5.0f;

        auto Hori = app->Keyboard.is_down(Key::W) ? 1.0f : app->Keyboard.is_down(Key::S) ? -1.0f : 0.0f;
        auto Vert = app->Keyboard.is_down(Key::D) ? 1.0f : app->Keyboard.is_down(Key::A) ? -1.0f : 0.0f;
        auto Up   = app->Keyboard.is_down(Key::M) ? 1.0f : app->Keyboard.is_down(Key::N) ? -1.0f : 0.0f;

        auto by = speed * delta;

        MainScene.main_camera().move({ Vert * by, Up * by, Hori * by });
    }
};




// no toche code
extern "C" {

    GAME_EXPORT auto game_ctor() -> void*
    {
        return new Game();
    }

    GAME_EXPORT auto game_dtor(void* game) -> void
    {
        delete static_cast<Game*>(game);
    }

    GAME_EXPORT auto game_link(void** funcs) -> void
    {
        gl::import_opengl_functions(funcs);
    }

    GAME_EXPORT auto game_set_app(APP* app_) -> void
    {
        ::app = app_;
    }

    GAME_EXPORT auto game_update(void* game, float delta) -> void
    {
        static_cast<Game*>(game)->update(delta);
    }

    GAME_EXPORT auto game_on_deltamouse(void* game, float dx, float dy) -> void
    {
        static_cast<Game*>(game)->on_deltamouse(dx, dy);
    }

}