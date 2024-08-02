#include <iostream>
#include "Window.hpp"
#include "Global_H.hpp"
#include "G_vars.hpp"
#include "Mesh.hpp"
#include "APP.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Camera.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include "Utils.hpp"
using namespace std;

void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
class Game : public APP
{
    
private:
    std::vector<Vertex> CubeVert {
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
        Vertex{ { -1.0f, 0.0f,  1.0f  }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f }  }, // Vertex 1
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
    Mesh cube;
    Mesh Plane;
    glm::mat4 Modle;
    glm::mat4 Perspective ;
    // glm::mat4 CameraMat;
    Shader DefaultShader;
public: // init here 
    Game()
    : cube({CubeVert, indices}), Plane({PlanVert, PlanIndices}), Modle(1.0f), DefaultShader(SHADER(Traingl))
    {
        cube.setupMesh();
        Plane.setupMesh();
        Perspective = glm::perspective(glm::radians(45.0f),(float)m_Window.GetWidth()/(float)m_Window.GetHeight(), 0.1f, 100.0f);
        DefaultShader.SetUniform("Perspective", Perspective);
        DefaultShader.SetUniform("Camera", Cam.GetViewMat());
        DefaultShader.SetUniform("Modle", Modle);
        unsigned int texture;
        glGenTextures(1, &texture);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        int width, height, nrChannels;
        stbi_set_flip_vertically_on_load(true);  
        unsigned char *data = stbi_load("C:\\Users\\sefyan\\Documents\\c_projects\\test\\Learn-Opengl\\res\\brik.jpg", &width, &height, &nrChannels, 0); // if .png GL_RGBA
        if (data)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
        {
            ERR("Failed to load Texture");
        }
        stbi_image_free(data);
        DefaultShader.Use();
        DefaultShader.SetUniform("ourTexture", 0);
        
    }
public:

    void Update(float delta) override {
        float WidrhOverHeight = m_Window.GetWidth()/ m_Window.GetHeight();
        Perspective = glm::perspective(glm::radians(45.0f), WidrhOverHeight, 0.1f, 100.0f);
        DefaultShader.SetUniform("Perspective", Perspective);

        Plane.Draw(DefaultShader);
        cube.Draw(DefaultShader);
        auto op = m_Window.mouse.ReadRawDelta();
        if(op){
            // glm::vec3 del = glm::vec3(op.value().x, op.value().y , 0) * 50.0f;
            LOG(op.value().x << " | " << op.value().y);
        auto ret = glm::rotate(Cam.GetViewMat(),  glm::radians(1.0f) * delta * op.value().x, {0,1,0});
        DefaultShader.SetUniform("Camera", ret);
        Cam.SetViewMat(ret);
        
        ret = glm::rotate(Cam.GetViewMat(),  glm::radians(1.0f) * delta * op.value().y, {1,0,0});
        DefaultShader.SetUniform("Camera", ret);
        Cam.SetViewMat(ret);

        // glm::vec2 MousePosClip = {MouseXFromWindowToOpengl(m_Window), MouseYFromWindowToOpengl(m_Window)};
        // LOG(MousePosClip.x << ", " << MousePosClip.y);
        }

        if( m_Window.kbd.KeyIsPressed('W')){
            Cam.MoveFroward(5.0f * delta);
            DefaultShader.SetUniform("Camera", Cam.GetViewMat());
       }
        else if( m_Window.kbd.KeyIsPressed('S')){
            Cam.MoveBackward(5.0f * delta);
            DefaultShader.SetUniform("Camera", Cam.GetViewMat());
       }
       else if( m_Window.kbd.KeyIsPressed('A')){
            Cam.MoveLeft(5.0f * delta);
            DefaultShader.SetUniform("Camera", Cam.GetViewMat());
       }
       else if( m_Window.kbd.KeyIsPressed('D')){
            Cam.MoveRight(5.0f * delta);
            DefaultShader.SetUniform("Camera", Cam.GetViewMat());
       }
       else if( m_Window.kbd.KeyIsPressed('N') ){
            Cam.MoveUP(5.0f * delta);
            DefaultShader.SetUniform("Camera", Cam.GetViewMat());
       }
       else if( m_Window.kbd.KeyIsPressed('M') ){
            Cam.MoveDown(5.0f * delta);
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