
#include <memory>
#include <random>

#include <core/Mesh.hpp>
#include <core/ResourceManager.hpp>
#include <core/APP.hpp>
#include <core/GameObject.hpp>
#include <core/Texture.hpp>
#include <core/Shader.hpp>
#include <core/ShaderProgram.hpp>
#include <core/Material.hpp>
#include <core/Scene.hpp>
#include <core/Log.hpp>

#include <cmrc/cmrc.hpp>
CMRC_DECLARE(res);
inline auto fs = cmrc::res::get_filesystem();

using namespace std;


/// @brief Game class hire the client put the logic for the game all the variables and stastes
class Game : public APP
{
private:
    std::shared_ptr<Shader> vert, frag;
    std::shared_ptr<ShaderProgram> CubeProgram;

    Scene MainScene;
public:
    /// @brief Get called at start of the game
    Game()
        : vert(std::make_shared<Shader>(fs.open("res/Shaders/cube.vert"), GL_VERTEX_SHADER))
        , frag(std::make_shared<Shader>(fs.open("res/Shaders/cube.frag"), GL_FRAGMENT_SHADER))
        , CubeProgram(std::make_shared<ShaderProgram>(vert, frag))
        , MainScene(ViewCamera)
    {

        ResManager["brik.jpg"]          = Texture2D(fs.open("res/brik.jpg"));
        ResManager["brik.png"]          = Texture2D(fs.open("res/brik.png"));
        ResManager["kimberley.jpg"]     = Texture2D(fs.open("res/kimberley.jpg"));
        ResManager["annie_spratt.jpg"]  = Texture2D(fs.open("res/annie_spratt.jpg"));
        ResManager["sand.png"]          = Texture2D(fs.open("res/gravelly_sand_diff_4k.png"));

        ResManager["forest.jpg"]= TextureCubeMap(TextureCubeMap::base_to_6facesfiles("res/forest.jpg"));

        ResManager["CubeMattkimberley"]  = Material(ResManager["kimberley.jpg"]);
        ResManager["CubeMattSand"]  = Material(ResManager["sand.png"]);
        ResManager["cubeMesh"]      = Mesh(Mesh::CUBE);
        ResManager["manMesh"]       = Mesh(obj_to_mesh(fs.open("res/FinalBaseMesh.obj")));

        constexpr int32_t Grids = 4;

        static thread_local std::mt19937 rng(std::random_device{}());
        static thread_local std::bernoulli_distribution coin(0.5f);

        for(int32_t i = -Grids; i < Grids; i ++){
            for(int32_t j = -Grids; j < Grids; j ++){
                auto m = coin(rng) ? ResManager["CubeMattkimberley"] : ResManager["CubeMattSand"];
                MainScene << GameObject(Transform({i, 0, j}, {0, 0, 0}, { 0.5f, 0.5f, 0.5f}), m, ResManager["cubeMesh"]);
            }
        }
        MainScene << GameObject(Transform({0, 0, 0}, {0, 0, 0}, { 0.2f, 0.2f, 0.2f}), ResManager["CubeMattSand"], ResManager["manMesh"]);
        MainScene << GameObject(Transform({2, 0, 0}, {0, 0, 0}, { 0.2f, 0.2f, 0.2f}), ResManager["CubeMattkimberley"], ResManager["manMesh"]);

        MainScene.set_skybox(ResManager["forest.jpg"]);
        debug::print("Window title: {}", Window.get_title());
    }
public:
    /// @brief Run every frame at 1/delta fps 
    /// @param delta  inverse of fps how mush time took a frame to Render
    auto update(float delta) -> void override {
        float speed = Keyboard.is_down(Key::LeftShift)? 10.0f : 5.0f;

        auto Hori = Keyboard.is_down(Key::W) ? 1.0f : Keyboard.is_down(Key::S) ? -1.0f : 0.0f;
        auto Vert = Keyboard.is_down(Key::D) ? 1.0f : Keyboard.is_down(Key::A) ? -1.0f : 0.0f;
        auto Up   = Keyboard.is_down(Key::M) ? 1.0f : Keyboard.is_down(Key::N) ? -1.0f : 0.0f;

        auto by = speed * delta;

        ViewCamera.move({ Vert * by, Up * by, Hori * by });

        render(MainScene, CubeProgram);

        Window.set_title(std::format("{}", smooth_fps()));
    }

public:

    /// @brief Get called when the game quit
    ~Game(){
        debug::print("Game Destructor");
    }
};


int main() {
    try {
        Game my_game;
        my_game.run();

    } catch(const CException& e) {
        debug::print(e.all());

    } catch(const std::exception& e) {
        debug::print(e.what());

    } catch(...) {
        debug::print("Unknown Exception");
    }
    return 0;
}