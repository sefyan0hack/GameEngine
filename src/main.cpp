#include <core/Mesh.hpp>
#include <core/APP.hpp>
#include <core/GameObject.hpp>
#include <core/Camera.hpp>
#include <core/Texture.hpp>
#include <core/Scene.hpp>
#include <core/Renderer.hpp>
#include <core/Shader.hpp>
#include <core/ShaderProgram.hpp>
#include <core/Material.hpp>
#include <core/Log.hpp>
#include <core/ResourceManager.hpp>


using namespace std;
class Game : public APP
{
private:
    std::shared_ptr<Shader> vert, frag;
    std::shared_ptr<ShaderProgram> CubeProgram;

    std::shared_ptr<Material> Matt;
    std::shared_ptr<Material> Matt2;
    std::shared_ptr<Mesh> cubeMesh;

    Scene Scn;
    Renderer rndr;
    public: // init here
    Game()
        : vert(std::make_shared<Shader>(SHADER(cube)".vert", GL_VERTEX_SHADER))
        , frag(std::make_shared<Shader>(SHADER(cube)".frag", GL_FRAGMENT_SHADER))
        , CubeProgram(std::make_shared<ShaderProgram>(vert, frag))
        , Matt(std::make_shared<Material>(CubeProgram))
        , Matt2(std::make_shared<Material>(CubeProgram))
        , cubeMesh(std::make_shared<Mesh>(Mesh::CUBE))
    {
        ResManager.load(TEXTURE(brik.jpg), ResType::Texture2D);
        ResManager.load(TEXTURE(brik.png), ResType::Texture2D);
        ResManager.load(TEXTURE(annie_spratt.jpg), ResType::Texture2D);
        ResManager.load(TEXTURE(gravelly_sand_diff_4k.png), ResType::Texture2D);

        // constexpr int32_t Grids = 5;

        // for(int32_t i = -Grids; i < Grids; i ++){
        //     for(int32_t j = -Grids; j < Grids; j ++){
        //         Scn << GameObject({i, 0, j}, Matt, cubeMesh);
        //     }
        // }

        Scn.setSkyBox(TEXTURE(forest.jpg));
        // Matt->SetDiffuse(ResManager.getTexture(TEXTURE(brik.png)));

        Scn << GameObject({0,0,0}, Matt, cubeMesh);

        Matt->SetDiffuse(ResManager.getTexture(TEXTURE(annie_spratt.jpg)));
        Scn << GameObject({0,1,0}, Matt, cubeMesh);

        Scn << GameObject({1,0,0}, Matt, cubeMesh);
        Scn << GameObject({0,0,1}, Matt, cubeMesh);
        Scn << GameObject({1,1,0}, Matt, cubeMesh);

        Matt2->SetDiffuse(ResManager.getTexture(TEXTURE(gravelly_sand_diff_4k.png)));
        Scn << GameObject({1,1,1}, Matt2, cubeMesh);

        Matt->SetTexture("uSkyboxMap", Scn.skyBox()->texture());
    }
public:

    auto Update(float delta) -> void override {
        float speed = 10.0f;

        if(Keyboard.IsDown(Key::LeftShift))
            speed *= 2;

        auto Hori = Keyboard.IsDown(Key::W) ? 1 : Keyboard.IsDown(Key::S) ? -1 : 0;
        auto Vert = Keyboard.IsDown(Key::D) ? 1 : Keyboard.IsDown(Key::A) ? -1 : 0;
        auto Up   = Keyboard.IsDown(Key::M) ? 1 : Keyboard.IsDown(Key::N) ? -1 : 0;

        auto by = speed * delta;

        ViewCamera.Move({ Vert * by, Up * by, Hori * by });

        rndr.render(Scn, ViewCamera);
    }

public: // distroy hire
    ~Game(){
        Info("Game Destructor");
    }
};


int main() {
    Game my_game ;
    my_game.Run();
    return 0;
}