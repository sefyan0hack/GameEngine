#include <iostream>
#include "Window.hpp"
#include "Global_H.hpp"
#include "G_vars.hpp"
#include "Mesh.hpp"
#include "APP.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
using namespace std;

void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
class Game : public APP
{
    
private:
    std::vector<Vertex> vertices {
        // Vertex                       // Normal             // TexCord
        Vertex{ { 0.5f,  0.5f,  0.5f }, { 1.0f, 0.0f, 1.0f }, { 0.0f, 0.0f }  }, // Vertex 1
        Vertex{ {-0.5f,  0.5f, -0.5f }, { 1.0f, 0.0f, 1.0f }, { 0.0f, 1.0f }  }, // Vertex 2
        Vertex{ {-0.5f,  0.5f,  0.5f }, { 1.0f, 0.0f, 1.0f }, { 1.0f, 0.0f }  }, // Vertex 3
        Vertex{ { 0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f }  }, // Vertex 4
        Vertex{ {-0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f }  }, // Vertex 5
        Vertex{ { 0.5f,  0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f }  }, // Vertex 6
        Vertex{ { 0.5f, -0.5f,  0.5f }, { 1.0f, 0.0f, 1.0f }, { 1.0f, 0.0f }  }, // Vertex 7
        Vertex{ {-0.5f, -0.5f,  0.5f }, { 1.0f, 0.0f, 1.0f }, { 1.0f, 1.0f }  }, // Vertex 8
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
    Mesh cube;
    glm::mat4 trans;
    glm::mat4 pers ;
    Shader pshad;
public: // init here 
    Game()
    : cube({vertices, indices}), trans(glm::mat4(1.0f)), pshad(SHADER(Traingl))
    {
        cube.setupMesh();
        trans = glm::rotate(trans, glm::radians(10.0f), glm::vec3(Rand_float, Rand_float, Rand_float));
        pers = glm::perspective(glm::radians(45.0f),(float)m_Window.GetWidth()/(float)m_Window.GetHeight(), 0.1f, 10.0f);
        pshad.SetUniform("persp", trans);
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
        pshad.Use();
        pshad.SetUniform("ourTexture", 0);
    }
public:

    void Update(float delta) override {
        auto op = m_Window.mouse.ReadRawDelta();
        if(op){
            auto d = op.value();

            if(m_Window.mouse.LeftIsPressed()){
                // Convert mouse delta to radians for rotation
                float angleX = glm::radians((float)d.x);
                float angleY = glm::radians((float)d.y);

                // Rotation axis (could be changed based on your application's needs)
                glm::vec3 axisX(1.0f, 0.0f, 0.0f); // Rotate around X-axis
                glm::vec3 axisY(0.0f, 1.0f, 0.0f); // Rotate around Y-axis

                // Apply rotation to the transformation matrix
                trans = glm::rotate(trans, angleX, axisY * delta);
                trans = glm::rotate(trans, angleY, axisX * delta);

                // Send the updated transformation matrix to the shader
                pshad.SetUniform("u_mat", trans);
            }
        }
        pshad.SetUniform("persp", pers);
        cube.Draw(pshad);

       if( m_Window.kbd.KeyIsPressed('A') || m_Window.kbd.KeyIsPressed(VK_LEFT)){
            trans = glm::translate(trans, glm::vec3(-1.0 * delta, 0.0, 0.0));
            pshad.SetUniform("u_mat", trans);


       }else if( m_Window.kbd.KeyIsPressed('D') || m_Window.kbd.KeyIsPressed(VK_RIGHT) ){
            trans = glm::translate(trans, glm::vec3(1.0 * delta, 0.0, 0.0));
            pshad.SetUniform("u_mat", trans);

       }else if( m_Window.kbd.KeyIsPressed('W') || m_Window.kbd.KeyIsPressed(VK_UP)){
            trans = glm::translate(trans, glm::vec3(0.0, 1.0 * delta, 0.0));
            pshad.SetUniform("u_mat", trans);

       }
       else if( m_Window.kbd.KeyIsPressed('S') || m_Window.kbd.KeyIsPressed(VK_DOWN) ){
            trans = glm::translate(trans, glm::vec3(0.0, -1.0 * delta, 0.0));
            pshad.SetUniform("u_mat", trans);

       }else if( m_Window.kbd.KeyIsPressed(VK_SPACE) ){
            trans = glm::translate(trans, glm::vec3(0.0, 0.0, -1.0 * delta ));
            pshad.SetUniform("u_mat", trans);

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