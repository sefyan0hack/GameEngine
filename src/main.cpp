#include <iostream>
#include <vector>
#include "Window.hpp"
#include "Global_H.hpp"
#include "G_vars.hpp"
#include "Mesh.hpp"
#include "APP.hpp"
#include "GameObject.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Camera.hpp"
#include "Texture.hpp"
#include <stb/stb_image.h>

#include "Utils.hpp"
using namespace std;

void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
class Game : public APP
{
    
private:
    std::vector<Vertex> cubeMeshVert {
        // Vertex                       // Normal             // TexCord
        Vertex{ { 0.5f,  0.5f,  0.5f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f }  }, // Vertex 1
        Vertex{ {-0.5f,  0.5f, -0.5f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f }  }, // Vertex 2
        Vertex{ {-0.5f,  0.5f,  0.5f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f }  }, // Vertex 3
        Vertex{ { 0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f }  }, // Vertex 4
        Vertex{ {-0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f }  }, // Vertex 5
        Vertex{ { 0.5f,  0.5f, -0.5f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f }  }, // Vertex 6
        Vertex{ { 0.5f, -0.5f,  0.5f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f }  }, // Vertex 7
        Vertex{ {-0.5f, -0.5f,  0.5f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f }  }, // Vertex 8
    };
    std::vector<Vertex> PlanVert {
        // Vertex                       // Normal             // TexCord
        Vertex{ { -1.0f, 0.0f,  1.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f }  }, // Vertex 1
        Vertex{ { 1.0f, 0.0f,  1.0f  }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f }  }, // Vertex 2
        Vertex{ { 1.0f, 0.0f, -1.0f  }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f }  }, // Vertex 3
        Vertex{ { -1.0f, 0.0f, -1.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f }  }, // Vertex 4
    };
    std::vector<GLuint>  PlanIndices = {
        0, 1, 2, // First Triangle
        0, 2, 3  // Second Triangle
    };
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
    Camera Cam;
    // vector<Mesh> Objects;
    vector<GameObject> Objects;
    Mesh cubeMesh;
    glm::mat4 Modle;
    glm::mat4 Perspective;
    // glm::mat4 CameraMat;
    Shader DefaultShader;
public: // init here
    Game()
    : Cam(m_Window.GetWidth(), m_Window.GetHeight(), m_Window.mouse.GetPos()), 
      cubeMesh({cubeMeshVert, indices}), Modle(1.0f), DefaultShader(SHADER(Traingl))
    {
        constexpr int Grids = 200;
        std::vector<glm::vec3> positions;

        for(int i = -Grids; i < Grids; i ++){
            for(int j = -Grids; j < Grids; j ++){
                for(int k = 0; k < 1 + rand() % 6; k ++)
                    positions.emplace_back(i, k % 2 , j);
            }
        }
        Objects.emplace_back(glm::vec3(0,0,0), DefaultShader, cubeMesh);
        for(auto &&Obj : Objects){
            Obj.SetUp(positions);
        }

        Perspective = glm::perspective(glm::radians(45.0f),(float)m_Window.GetWidth()/(float)m_Window.GetHeight(), 0.1f, 100.0f);
        DefaultShader.SetUniform("Perspective", Perspective);
        DefaultShader.SetUniform("Camera", Cam.GetViewMat());
        Texture brik ;
        brik.Loud(TEXTURE(brik.jpg));
        brik.BindByName(TEXTURE(brik.jpg));
        DefaultShader.Use();
        DefaultShader.SetUniform("ourTexture", 0);
        m_Window.mouse.SetPos(m_Window.GetWidth()/2.0f, m_Window.GetHeight()/2.0f);
    }
public:

    void Update(float delta) override {
        float WidrhOverHeight = (float)m_Window.GetWidth()/ m_Window.GetHeight();
        Perspective = glm::perspective(glm::radians(45.0f), WidrhOverHeight, 0.1f, 100.0f);
        DefaultShader.SetUniform("Perspective", Perspective);

        //Drwaing
        for(auto &obj: Objects){
            obj.Render();
        }

        Cam.MoseMove(m_Window.mouse);
        m_Window.mouse.SetPos(m_Window.GetWidth()/2.0f, m_Window.GetHeight()/2.0f);
        
        DefaultShader.SetUniform("Camera", Cam.GetViewMat());

        if( m_Window.kbd.KeyIsPressed('W')){
            float speed = 10.0f * delta;
            if(m_Window.kbd.KeyIsPressed(VK_SHIFT))
                speed *= 10;
                
            Cam.MoveFroward(speed);
            DefaultShader.SetUniform("Camera", Cam.GetViewMat());
       }
        else if( m_Window.kbd.KeyIsPressed('S')){
            Cam.MoveBackward(10.0f * delta);
            DefaultShader.SetUniform("Camera", Cam.GetViewMat());
       }
       else if( m_Window.kbd.KeyIsPressed('A')){
            Cam.MoveLeft(10.0f * delta);
            DefaultShader.SetUniform("Camera", Cam.GetViewMat());
       }
       else if( m_Window.kbd.KeyIsPressed('D')){
            Cam.MoveRight(10.0f * delta);
            DefaultShader.SetUniform("Camera", Cam.GetViewMat());
       }
       else if( m_Window.kbd.KeyIsPressed('N') ){
            Cam.MoveUP(10.0f * delta);
            DefaultShader.SetUniform("Camera", Cam.GetViewMat());
       }
       else if( m_Window.kbd.KeyIsPressed('M') ){
            Cam.MoveDown(10.0f * delta);
            DefaultShader.SetUniform("Camera", Cam.GetViewMat());
       }

       if( m_Window.kbd.KeyIsPressed(VK_UP)){
            Modle = glm::translate(Modle, {0, 1 * delta, 0});
            DefaultShader.SetUniform("Modle", Modle);
       }
        else if(m_Window.kbd.KeyIsPressed(VK_DOWN) ){
            Modle = glm::translate(Modle, {0, -1 * delta, 0});
            DefaultShader.SetUniform("Modle", Modle);
       }
       else if( m_Window.kbd.KeyIsPressed(VK_LEFT) ){
            Modle = glm::translate(Modle, {-1 * delta, 0, 0});
            DefaultShader.SetUniform("Modle", Modle);
       }
       else if( m_Window.kbd.KeyIsPressed(VK_RIGHT) ){
            Modle = glm::translate(Modle, {1 * delta, 0, 0});
            DefaultShader.SetUniform("Modle", Modle);
       }
       LOG( "Fps : " << this->fps.QuadPart);
    }

public: // distroy hire
    ~Game(){
        LOG("Game destryed");
    }
};


int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int ) {
    Game my_game ;
    my_game.Run();
    return 0;
}