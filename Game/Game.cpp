
#include <memory>
#include <random>

#include <Engine.hpp>

using namespace std;

class Game final : IGame
{
public:
    Game()
    {
        auto kimberley_jpg     = Texture::texture_2d("res/textures/kimberley.jpg");
        auto sand_png          = Texture::texture_2d("res/textures/gravelly_sand_diff_4k.png");
        auto heli_jpg          = Texture::texture_2d("res/textures/heli.jpg");

        auto CubeMattkimberley = std::make_shared<Material>(kimberley_jpg);
        auto CubeMattSand  = std::make_shared<Material>(sand_png);
        auto heliMatt  = std::make_shared<Material>(heli_jpg);
        auto cubeMesh      = std::make_shared<Mesh>(Mesh::CUBE_VERTICES(), Mesh::CUBE_INDICES());
        auto manMesh       = std::make_shared<Mesh>(load_obj("res/models/FinalBaseMesh.obj"));
        auto heli       = std::make_shared<Mesh>(load_obj("res/models/heli.obj"));

        constexpr int32_t Grids = 5;

        std::mt19937 rng(std::random_device{}());
        std::bernoulli_distribution coin(0.5f);

        for(int32_t i = -Grids; i < Grids; i ++){
            for(int32_t j = -Grids; j < Grids; j ++){
                auto m = coin(rng) ? CubeMattkimberley : CubeMattSand;
                auto t = Transform({float(i), 0, float(j)}, {0, 0, 0}, { 0.5f, 0.5f, 0.5f});
            
                Scene << GameObject(t, m, cubeMesh);
            }
        }

        Scene << GameObject(Transform({0, 0, 0}, {0, 0, 0}, { 0.2f, 0.2f, 0.2f}), CubeMattSand, manMesh);
        Scene << GameObject(Transform({2, 0, 0}, {0, 0, 0}, { 0.2f, 0.2f, 0.2f}), CubeMattkimberley, manMesh);
        Scene << GameObject(Transform({6, 6, 6}, {0, 0, 0}, { 1, 1, 1}), heliMatt, heli);
    }

    auto update(float delta) -> void override
    {
        camera_mouvment(delta);
    }

    auto on_deltamouse(float dx, float dy) -> void override
    {
        Scene.main_camera().process_mouse_movement(dx, -dy);
    }

    auto camera_mouvment(float delta) -> void
    {
        float speed = Keyboard.is_down(Key::LeftShift)? 10.0f : 5.0f;

        auto Hori = Keyboard.is_down(Key::W) ? 1.0f : Keyboard.is_down(Key::S) ? -1.0f : 0.0f;
        auto Vert = Keyboard.is_down(Key::D) ? 1.0f : Keyboard.is_down(Key::A) ? -1.0f : 0.0f;
        auto Up   = Keyboard.is_down(Key::M) ? 1.0f : Keyboard.is_down(Key::N) ? -1.0f : 0.0f;

        auto by = speed * delta;

        Scene.main_camera().move({ Vert * by, Up * by, Hori * by });
    }
}_;// note `_` at the end of Game class is an instance of Game
