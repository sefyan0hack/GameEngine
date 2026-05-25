
#include <memory>
#include <random>
#include <Engine.hpp>

extern cmrc::embedded_filesystem embed_filesystem;

using namespace std;

class Game : IGame
{
    APP& app = APP::self(this);

public:
    Game()
    {
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
            
                app.MainScene << GameObject(t, m, meshRes);
            }
        }
        app.MainScene << GameObject(Transform({0, 0, 0}, {0, 0, 0}, { 0.2f, 0.2f, 0.2f}), CubeMattSand, manMesh);
        app.MainScene << GameObject(Transform({2, 0, 0}, {0, 0, 0}, { 0.2f, 0.2f, 0.2f}), CubeMattkimberley, manMesh);

    }

    auto update(float delta) -> void override
    {
        app.UiText.text(std::format("FPS: {:.2f}", app.fps()), { 0, 0 });
        app.UiText.text(std::format("Memory: {}/{} MB", os::memory_usage(), os::memory_peak()), { 0, Text::FONT_SIZE });
        app.UiText.text(std::format("Threads: {}", std::thread::hardware_concurrency()), { 0, Text::FONT_SIZE * 2 });

        camera_mouvment(delta);
    } 

    auto on_deltamouse(float dx, float dy) -> void override
    {
        app.MainScene.main_camera().process_mouse_movement(dx, -dy);
    }

    auto camera_mouvment(float delta) -> void
    {
        float speed = app.Keyboard.is_down(Key::LeftShift)? 10.0f : 5.0f;

        auto Hori = app.Keyboard.is_down(Key::W) ? 1.0f : app.Keyboard.is_down(Key::S) ? -1.0f : 0.0f;
        auto Vert = app.Keyboard.is_down(Key::D) ? 1.0f : app.Keyboard.is_down(Key::A) ? -1.0f : 0.0f;
        auto Up   = app.Keyboard.is_down(Key::M) ? 1.0f : app.Keyboard.is_down(Key::N) ? -1.0f : 0.0f;

        auto by = speed * delta;

        app.MainScene.main_camera().move({ Vert * by, Up * by, Hori * by });
    }
}_;// note `_` at the end of Game class is an instance of Game
