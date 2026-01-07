
#include <memory>
#include <random>
#include <Engine.hpp>
#include <game_export.h>

APP* app;
extern cmrc::embedded_filesystem embed_filesystem;

using namespace std;


class GAME_EXPORT Game
{

public:
    Game()
    {
        // slow as fk right now 
        // debug::logger = [](std::string m){
        //     static auto l = 0;
        //     app->TextRenderer.text(m, { 0, l += TextRenderer::FONT_SIZE });
        // };

        Material::set_skybox(std::make_shared<TextureCubeMap>(TextureCubeMap::base_to_6facesfiles("res/forest.jpg")));

        auto kimberley_jpg     = std::make_shared<Texture2D>(embed_filesystem.open("res/kimberley.jpg"));
        auto sand_png          = std::make_shared<Texture2D>(embed_filesystem.open("res/gravelly_sand_diff_4k.png"));

        auto CubeMattkimberley = std::make_shared<Material>(kimberley_jpg);
        auto CubeMattSand  = std::make_shared<Material>(sand_png);
        auto cubeMesh      = std::make_shared<Mesh>(Mesh::CUBE);
        auto manMesh       = std::make_shared<Mesh>(obj_to_mesh(embed_filesystem.open("res/FinalBaseMesh.obj")));

        constexpr int32_t Grids = 5;

        std::mt19937 rng(std::random_device{}());
        std::bernoulli_distribution coin(0.5f);

        for(int32_t i = -Grids; i < Grids; i ++){
            for(int32_t j = -Grids; j < Grids; j ++){
                auto m = coin(rng) ? CubeMattkimberley : CubeMattSand;
                auto meshRes = cubeMesh;
                auto t = Transform({i, 0, j}, {0, 0, 0}, { 0.5f, 0.5f, 0.5f});
            
                app->MainScene << GameObject(t, m, meshRes);
            }
        }
        app->MainScene << GameObject(Transform({0, 0, 0}, {0, 0, 0}, { 0.2f, 0.2f, 0.2f}), CubeMattSand, manMesh);
        app->MainScene << GameObject(Transform({2, 0, 0}, {0, 0, 0}, { 0.2f, 0.2f, 0.2f}), CubeMattkimberley, manMesh);

        
        using namespace os::host;
        utils::async_repeat_every(1000,
            [](){
                app->TextRenderer.text(std::format("FPS: {:.2f}", app->fps()), { 0, 0 });
                app->TextRenderer.text(std::format("Memory: {}/{}", memory_usage(), memory_peak()), { 0, TextRenderer::FONT_SIZE });
            }
        );

        // app->TextRenderer.text(std::format("id: {}\nos: {}\narch: {}\nusage: {} Mb\npeak: {} Mb\nthread_count: {}", 
        //     proc_id(), name(), arch(), memory_usage(), memory_peak(), thread_count()), { 0, TextRenderer::FONT_SIZE });
        // }
        
        debug::log("{}", VecWrapper{gl::OPENGL_FUNCTIONS_NAME});
    }

    auto update(float delta) -> void
    {
        camera_mouvment(delta);
    } 

    auto on_deltamouse(float dx, float dy) -> void
    {
        app->MainScene.main_camera().process_mouse_movement(dx, -dy);
    }

    auto camera_mouvment(float delta) -> void
    {
        float speed = app->Keyboard.is_down(Key::LeftShift)? 10.0f : 5.0f;

        auto Hori = app->Keyboard.is_down(Key::W) ? 1.0f : app->Keyboard.is_down(Key::S) ? -1.0f : 0.0f;
        auto Vert = app->Keyboard.is_down(Key::D) ? 1.0f : app->Keyboard.is_down(Key::A) ? -1.0f : 0.0f;
        auto Up   = app->Keyboard.is_down(Key::M) ? 1.0f : app->Keyboard.is_down(Key::N) ? -1.0f : 0.0f;

        auto by = speed * delta;

        app->MainScene.main_camera().move({ Vert * by, Up * by, Hori * by });
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