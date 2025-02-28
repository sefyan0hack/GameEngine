#include <iostream>
#include <vector>
#include <stacktrace>
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
#include <thread>

// #define USE_EBO

using namespace std;
class Game : public APP
{
    
private:
#ifdef USE_EBO
    std::vector<Vertex> cubeMeshVert {
        // Vertex                       // Normal             // TexCord
        Vertex{ { 0.5f,  0.5f,  0.5f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f }  }, // Vertex 1
        Vertex{ {-0.5f,  0.5f, -0.5f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f }  }, // Vertex 2
        Vertex{ {-0.5f,  0.5f,  0.5f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f }  }, // Vertex 3
        Vertex{ { 0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f }  }, // Vertex 4
        Vertex{ {-0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f }  }, // Vertex 5
        Vertex{ { 0.5f,  0.5f, -0.5f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f }  }, // Vertex 6
        Vertex{ { 0.5f, -0.5f,  0.5f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f }  }, // Vertex 7
        Vertex{ {-0.5f, -0.5f,  0.5f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f }  }, // Vertex 8
    };
#else
    std::vector<Vertex> cubeMeshVert{
        // Front Face
        {{ 0.5f,  0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {1.0f, 1.0f}}, // Top Right
        {{-0.5f, -0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {0.0f, 0.0f}}, // Bottom Left
        {{-0.5f,  0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {0.0f, 1.0f}}, // Top Left
        {{ 0.5f,  0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {1.0f, 1.0f}}, // Top Right
        {{ 0.5f, -0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {1.0f, 0.0f}}, // Bottom Right
        {{-0.5f, -0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {0.0f, 0.0f}}, // Bottom Left

        // Back Face
        {{ 0.5f,  0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, {1.0f, 1.0f}}, // Top Right
        {{-0.5f,  0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, {0.0f, 1.0f}}, // Top Left
        {{-0.5f, -0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, {0.0f, 0.0f}}, // Bottom Left
        {{ 0.5f,  0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, {1.0f, 1.0f}}, // Top Right
        {{-0.5f, -0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, {0.0f, 0.0f}}, // Bottom Left
        {{ 0.5f, -0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, {1.0f, 0.0f}}, // Bottom Right

        // Left Face
        {{-0.5f,  0.5f,  0.5f}, {-1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}}, // Top Right
        {{-0.5f, -0.5f, -0.5f}, {-1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}}, // Bottom Left
        {{-0.5f,  0.5f, -0.5f}, {-1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}}, // Top Left
        {{-0.5f,  0.5f,  0.5f}, {-1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}}, // Top Right
        {{-0.5f, -0.5f,  0.5f}, {-1.0f,  0.0f,  0.0f}, {1.0f, 0.0f}}, // Bottom Right
        {{-0.5f, -0.5f, -0.5f}, {-1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}}, // Bottom Left

        // Right Face
        {{ 0.5f,  0.5f,  0.5f}, { 1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}}, // Top Left
        {{ 0.5f,  0.5f, -0.5f}, { 1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}}, // Top Right
        {{ 0.5f, -0.5f, -0.5f}, { 1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}}, // Bottom Right
        {{ 0.5f,  0.5f,  0.5f}, { 1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}}, // Top Left
        {{ 0.5f, -0.5f, -0.5f}, { 1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}}, // Bottom Right
        {{ 0.5f, -0.5f,  0.5f}, { 1.0f,  0.0f,  0.0f}, {1.0f, 0.0f}}, // Bottom Left

        // Top Face
        {{ 0.5f,  0.5f,  0.5f}, { 0.0f,  1.0f,  0.0f}, {1.0f, 1.0f}}, // Top Right
        {{-0.5f,  0.5f, -0.5f}, { 0.0f,  1.0f,  0.0f}, {0.0f, 0.0f}}, // Bottom Left
        {{-0.5f,  0.5f,  0.5f}, { 0.0f,  1.0f,  0.0f}, {0.0f, 1.0f}}, // Top Left
        {{ 0.5f,  0.5f,  0.5f}, { 0.0f,  1.0f,  0.0f}, {1.0f, 1.0f}}, // Top Right
        {{ 0.5f,  0.5f, -0.5f}, { 0.0f,  1.0f,  0.0f}, {1.0f, 0.0f}}, // Bottom Right
        {{-0.5f,  0.5f, -0.5f}, { 0.0f,  1.0f,  0.0f}, {0.0f, 0.0f}}, // Bottom Left

        // Bottom Face
        {{ 0.5f, -0.5f,  0.5f}, { 0.0f, -1.0f,  0.0f}, {1.0f, 1.0f}}, // Top Right
        {{-0.5f, -0.5f,  0.5f}, { 0.0f, -1.0f,  0.0f}, {0.0f, 1.0f}}, // Top Left
        {{-0.5f, -0.5f, -0.5f}, { 0.0f, -1.0f,  0.0f}, {0.0f, 0.0f}}, // Bottom Left
        {{ 0.5f, -0.5f,  0.5f}, { 0.0f, -1.0f,  0.0f}, {1.0f, 1.0f}}, // Top Right
        {{-0.5f, -0.5f, -0.5f}, { 0.0f, -1.0f,  0.0f}, {0.0f, 0.0f}}, // Bottom Left
        {{ 0.5f, -0.5f, -0.5f}, { 0.0f, -1.0f,  0.0f}, {1.0f, 0.0f}}, // Bottom Right
    };
#endif
    std::vector<GLuint> indices {
        0, 1, 2,
        1, 3, 4,
        5, 6, 3,
        7, 3, 6,
        2, 4, 7,
        0, 7, 6,
        0, 5, 1,
        1, 5, 3,
        5, 0, 6,
        7, 4, 3,
        2, 1, 4,
        0, 2, 7
    };
    Shader skyVert, skyfrag;
    Material skyMat;
    Mesh skyMesh;

    Shader vert, frag;
    Material Matt;
    Mesh cubeMesh;

    Camera Cam;
    Scene Scn;
    Renderer rndr;
    public: // init here
    Game()
    : skyVert(SHADER(skybox)".vert", GL_VERTEX_SHADER)
    , skyfrag(SHADER(skybox)".frag", GL_FRAGMENT_SHADER)
    , skyMat(skyVert, skyfrag)
    , skyMesh(cubeMeshVert)

    , vert(SHADER(Traingl)".vert", GL_VERTEX_SHADER)
    , frag(SHADER(Traingl)".frag", GL_FRAGMENT_SHADER)
    , Matt(vert, frag)
    , cubeMesh({cubeMeshVert, indices})
    , Cam(m_Window)
    {
        constexpr int Grids = 200;
        [[maybe_unused]] std::vector<glm::vec3> positions(Grids * Grids * 4);
        size_t index = 0;
        for(int i = -Grids; i < Grids; i ++){
            for(int j = -Grids; j < Grids; j ++)
                positions[index++] = {i, 0, j};
        }

        skyMat.SetTexture(std::vector<std::string>{
            TEXTURE(posx.jpg), TEXTURE(negx.jpg),
            TEXTURE(posy.jpg), TEXTURE(negy.jpg),
            TEXTURE(posz.jpg), TEXTURE(negz.jpg),
        });
        Scn.add({glm::vec3(0,0,0), skyMat, skyMesh});

        Matt.SetTexture(TEXTURE(brik.png));
        Scn.add({{0,0,0}, Matt, cubeMesh});
        // Scn.Entitys().back().SetUp(positions);

        Scn.add({{0,5,0}, Matt, cubeMesh});
        // Scn.Entitys().back().SetUp(positions);
        
        // auto mesh2 = Mesh(cubeMeshVert);
        // auto matt2 = Material(Shader(SHADER(Traingl)".vert", GL_VERTEX_SHADER), Shader(SHADER(Traingl)".frag", GL_FRAGMENT_SHADER));
        // matt2.texture(TEXTURE(brik.png));        

        // #define PrintGLfunc(type, name) Log::print("{} {}", (void*)name, #name)
        
        // GLFUNCS(PrintGLfunc)
    }
public:

    auto Update(float delta) -> void override {

        Cam.MoseMove();
        if( m_Window.m_Keyboard->KeyIsPressed('W')){
            float speed = 10.0f * delta;
            if(m_Window.m_Keyboard->KeyIsPressed(VK_SHIFT))
                speed *= 10;
                
            Cam.MoveFroward(speed);
        }
        if( m_Window.m_Keyboard->KeyIsPressed('S')){
            Cam.MoveBackward(10.0f * delta);
        }
        if( m_Window.m_Keyboard->KeyIsPressed('A')){
             Cam.MoveLeft(10.0f * delta);
        }
        if( m_Window.m_Keyboard->KeyIsPressed('D')){
             Cam.MoveRight(10.0f * delta);
        }
        if( m_Window.m_Keyboard->KeyIsPressed('N') ){
             Cam.MoveUP(10.0f * delta);
        }
        if( m_Window.m_Keyboard->KeyIsPressed('M') ){
             Cam.MoveDown(10.0f * delta);
        }
        if (m_Window.m_Keyboard->KeyIsPressed('H')){
            static bool flip = false;
            if(flip == false){
                flip = !flip;
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            }else{
                flip = !flip;
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
        }

        rndr.render(Scn, Cam);
        //    LOG( "Fps : " << this->fps.QuadPart);
    }

public: // distroy hire
    ~Game(){
        Log::Info("Game Destructor");
    }
};

auto main(void) -> int {
    setup_crach_handler();
    Game my_game ;
    my_game.Run();
    return 0;
}