#include <core/Mesh.hpp>
#include <core/APP.hpp>
#include <core/GameObject.hpp>
#include <core/Camera.hpp>
#include <core/Texture.hpp>
#include <core/Scene.hpp>
#include <core/Renderer.hpp>
#include <core/Shader.hpp>
#include <core/Material.hpp>
#include <core/Log.hpp>
#include <core/ResourceManager.hpp>


using namespace std;
class Game : public APP
{
private:
    Shader skyVert, skyfrag;
    Material skyMat;
    Mesh skyMesh;

    Shader vert, frag;
    Material Matt;
    Mesh cubeMesh;

    Scene Scn;
    Renderer rndr;
    public: // init here
    Game()
    : skyVert(SHADER(skybox)".vert", GL_VERTEX_SHADER)
    , skyfrag(SHADER(skybox)".frag", GL_FRAGMENT_SHADER)
    , skyMat(skyVert, skyfrag)
    , skyMesh(Mesh::CUBE)

    , vert(SHADER(cube)".vert", GL_VERTEX_SHADER)
    , frag(SHADER(cube)".frag", GL_FRAGMENT_SHADER)
    , Matt(vert, frag)
    , cubeMesh(Mesh::CUBE)
    {
        ResManager.load(TEXTURE(brik.jpg), ResType::Texture2D);
        ResManager.load(TEXTURE(brik.png), ResType::Texture2D);
        ResManager.load(TEXTURE(annie_spratt.jpg), ResType::Texture2D);
        ResManager.load(TEXTURE(gravelly_sand_diff_4k.png), ResType::Texture2D);
        ResManager.load(TEXTURE(forest.jpg), ResType::TextureCube);

        constexpr int32_t Grids = 200;
        [[maybe_unused]] std::vector<glm::vec3> positions(Grids * Grids * 4);
        std::size_t index = 0;
        for(int32_t i = -Grids; i < Grids; i ++){
            for(int32_t j = -Grids; j < Grids; j ++)
                positions[index++] = {i, 0, j};
        }

        skyMat.SetTexture("uDiffuseMap", ResManager.getTexture(TEXTURE(forest.jpg)));

        Scn.setSkyBox(std::make_unique<SkyBox>(skyMat));

        Matt.SetTexture("uDiffuseMap", ResManager.getTexture(TEXTURE(brik.png)));
        Scn << GameObject({0,0,0}, Matt, cubeMesh);

        Matt.SetTexture("uDiffuseMap", ResManager.getTexture(TEXTURE(annie_spratt.jpg)));
        Scn << GameObject({0,1,0}, Matt, cubeMesh);

        Matt.SetTexture("uDiffuseMap", ResManager.getTexture(TEXTURE(gravelly_sand_diff_4k.png)));
        Scn << GameObject({1,0,0}, Matt, cubeMesh);
        Scn << GameObject({0,0,1}, Matt, cubeMesh);
        Scn << GameObject({1,1,0}, Matt, cubeMesh);
        Scn << GameObject({1,1,1}, Matt, cubeMesh);

        Matt.SetTexture("uSkyboxMap", skyMat.texture("uDiffuseMap"));
    }
public:

    auto Update(float delta) -> void override {
        float speed = 10.0f;

        if(Keyboard.IsDown(Key::LeftShift))
                speed *= 2;
    
        if(Keyboard.IsDown(Key::W)){
            ViewCamera.MoveForward(speed * delta);
        }
        if(Keyboard.IsDown(Key::S)){
            ViewCamera.MoveBackward(speed * delta);
        }
        if(Keyboard.IsDown(Key::A)){
            ViewCamera.MoveLeft(speed * delta);
        }
        if(Keyboard.IsDown(Key::D)){
            ViewCamera.MoveRight(speed * delta);
        }
        if(Keyboard.IsDown(Key::N) ){
            ViewCamera.MoveUp(speed * delta);
        }
        if(Keyboard.IsDown(Key::M) ){
            ViewCamera.MoveDown(speed * delta);
        }

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