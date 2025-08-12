
#include <memory>
#include <random>

#include <core/Mesh.hpp>
#include <core/ResourceManager.hpp>
#include <core/APP.hpp>
#include <core/GameObject.hpp>
#include <core/SkyBox.hpp>
#include <core/Camera.hpp>
#include <core/Texture.hpp>
#include <core/Scene.hpp>
#include <core/Renderer.hpp>
#include <core/Shader.hpp>
#include <core/ShaderProgram.hpp>
#include <core/Material.hpp>
#include <core/Log.hpp>

using namespace std;

/// @brief Game class hire the client put the logic for the game all the variables and stastes
class Game : public APP
{
private:
    std::shared_ptr<Shader> vert, frag;
    std::shared_ptr<ShaderProgram> CubeProgram;
public:
    /// @brief Get called at start of the game
    Game()
        : vert(std::make_shared<Shader>(SHADER(cube)".vert", GL_VERTEX_SHADER))
        , frag(std::make_shared<Shader>(SHADER(cube)".frag", GL_FRAGMENT_SHADER))
        , CubeProgram(std::make_shared<ShaderProgram>(vert, frag))
    {

        ResManager["brik.jpg"]          = Texture2D(TEXTURE(brik.jpg));
        ResManager["brik.png"]          = Texture2D(TEXTURE(brik.png));
        ResManager["kimberley.jpg"]     = Texture2D(TEXTURE(kimberley.jpg));
        ResManager["annie_spratt.jpg"]  = Texture2D(TEXTURE(annie_spratt.jpg));
        ResManager["sand.png"]          = Texture2D(TEXTURE(gravelly_sand_diff_4k.png));

        ResManager["forest.jpg"]= TextureCubeMap(TextureCubeMap::base_to_6faces(TEXTURE(forest.jpg)));

        ResManager["cube.vert"] = Shader(SHADER(cube)".vert", GL_VERTEX_SHADER);

        ResManager["CubeMattkimberley"]  = Material(ResManager["kimberley.jpg"]);
        ResManager["CubeMattSand"]  = Material(ResManager["sand.png"]);
        ResManager["cubeMesh"]      = Mesh(Mesh::CUBE);
        ResManager["manMesh"]       = Mesh(Obj2Mesh(TEXTURE(FinalBaseMesh.obj)));

        constexpr int32_t Grids = 10;

        static thread_local std::mt19937 rng(std::random_device{}());
        static thread_local std::bernoulli_distribution coin(0.5f);

        for(int32_t i = -Grids; i < Grids; i ++){
            for(int32_t j = -Grids; j < Grids; j ++){
                auto m = coin(rng) ? ResManager["CubeMattkimberley"] : ResManager["CubeMattSand"];
                MainScene << GameObject(Transform({i, 0, j}, {0, 0, 0}, { 0.5f, 0.5f, 0.5f}), m, ResManager["cubeMesh"]);
                // MainScene << GameObject(Transform({i, 0, j}, {0, 0, 0}, { 0.2f, 0.2f, 0.2f}), m, ResManager["manMesh"]);
            }
        }

        MainScene.SetSkyBox(ResManager["forest.jpg"]);

        ResManager["CubeMattkimberley"].get<Material>()->SetTexture("uSkyboxMap", MainScene.SkyBox()->texture());
        ResManager["CubeMattSand"].get<Material>()->SetTexture("uSkyboxMap", MainScene.SkyBox()->texture());
    }
public:
    /// @brief Run every frame at 1/delta fps
    /// @param delta  inverse of fps how mush time took a frame to Render
    auto Update(float delta) -> void override {
        float speed = Keyboard.IsDown(Key::LeftShift)? 20.0f : 10.0f;

        auto Hori = Keyboard.IsDown(Key::W) ? 1.0f : Keyboard.IsDown(Key::S) ? -1.0f : 0.0f;
        auto Vert = Keyboard.IsDown(Key::D) ? 1.0f : Keyboard.IsDown(Key::A) ? -1.0f : 0.0f;
        auto Up   = Keyboard.IsDown(Key::M) ? 1.0f : Keyboard.IsDown(Key::N) ? -1.0f : 0.0f;

        auto by = speed * delta;

        ViewCamera.Move({ Vert * by, Up * by, Hori * by });

        Render(MainScene, CubeProgram);
    }

public:

    /// @brief Get called when the game quit
    ~Game(){
        Debug::Print("Game Destructor");
    }
};


int main() {
    try {
        Game my_game;
        my_game.Run();

    } catch(const CoreException& e) {
        std::cerr << e.all() << '\n';

    } catch(const std::exception& e) {
        std::cerr << e.what() << '\n';

    } catch(...) {
        std::cerr << "Unknown Exception"<< '\n';
    }
    return 0;
}