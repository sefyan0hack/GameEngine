#include <iostream>
#include <windows.h>
#include "Window.hpp"
#include "Global_H.hpp"
#include "G_vars.hpp"
#include "VAO.hpp"
#include "VBO.hpp"
#include "Shader.hpp"
#include "APP.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace std;

class Game : public APP
{
    private:
    std::vector<float> vertices {
        -0.25, -0.25, -0.25, Rand_float, Rand_float, Rand_float, // Vertex 1
        0.25, -0.25, -0.25,  Rand_float, Rand_float, Rand_float, // Vertex 2
        -0.25, 0.25, -0.25,  Rand_float, Rand_float, Rand_float, // Vertex 3
        0.25, 0.25, -0.25,   Rand_float, Rand_float, Rand_float, // Vertex 4
        -0.25, -0.25, 0.25,  Rand_float, Rand_float, Rand_float, // Vertex 5
        0.25, -0.25, 0.25,   Rand_float, Rand_float, Rand_float, // Vertex 6
        -0.25, 0.25, 0.25,   Rand_float, Rand_float, Rand_float, // Vertex 7
        0.25, 0.25, 0.25    ,Rand_float, Rand_float, Rand_float, // Vertex 8
        // -0.5f, -0.5f, 0.0f,
        //  0.5f, -0.5f, 0.0f,
        //  0.0f,  0.5f, 0.0f
    };
    std::vector<GLuint> ebo_indices{
        // # Front face
        4, 5, 6,
        5, 7, 6,
        // # Back face
        0, 1, 2,
        1, 3, 2,
        // # Left face
        0, 4, 2,
        4, 6, 2,
        // # Right face
        1, 5, 3,
        5, 7, 3,
        // # Top face
        2, 3, 6,
        3, 7, 6,
        // # Bottom face
        0, 1, 4,
        1, 5, 4
    };
    GLuint ebo = 0;
    VAO vao;
    VBO vbo;
    Shader Tring;
    GLuint mLoc;
    glm::mat4 trans;
    
public:
    void Start() override{
        trans = glm::mat4(1.0f);
        vao.Bind(); // 1 - bind vao
        vbo.Bind(); // 2 - bind vbo 
        glGenBuffers(1, &ebo);
        vbo.UpData(vertices); // 3 - up data to vbo
        // rmember the stride is grouing for VAO but the same for all attribs
        vao.SetLout(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); // 4 - set attribs
        vao.SetLout(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3*sizeof(float))); // 4 - set attribs
        // vbo.UnBind(); // 5 - unbind if you want  vbo  (not vao)
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, ebo_indices.size() * sizeof(GLuint), ebo_indices.data(), GL_STATIC_DRAW);

        Tring.Load(SHADER(Traingl));
        Tring.UseProgram();
        mLoc = glGetUniformLocation(Tring.GetProgram(), "u_mat");
        trans = glm::rotate(trans, glm::radians(10.0f), glm::vec3(Rand_float, Rand_float, Rand_float));
        // The first argument should be familiar by now which is the uniform's location. The second argument tells OpenGL how many matrices we'd like to send, which is 1. The third argument asks us if we want to transpose our matrix, that is to swap the columns and rows. OpenGL developers often use an internal matrix layout called column-major ordering which is the default matrix layout in GLM so there is no need to transpose the matrices; we can keep it at GL_FALSE. The last parameter is the actual matrix data, but GLM stores their matrices' data in a way that doesn't always match OpenGL's expectations so we first convert the data with GLM's built-in function value_ptr.
        glUniformMatrix4fv(mLoc, 1, GL_FALSE, glm::value_ptr(trans));
        
        LOG("Game started");
        m_Window.kbd.EnableAutorepeat();
    }
    void Update(float delta) override{
        // glDrawArrays(GL_TRIANGLES, 0, 3);
        glDrawElements(GL_TRIANGLES, ebo_indices.size(), GL_UNSIGNED_INT, 0);

       if( m_Window.kbd.KeyIsPressed('A') || m_Window.kbd.KeyIsPressed(VK_LEFT)){
            trans = glm::translate(trans, glm::vec3(-1.0 * delta, 0.0, 0.0));
            glUniformMatrix4fv(mLoc, 1, GL_FALSE, glm::value_ptr(trans));

       }else if( m_Window.kbd.KeyIsPressed('D') || m_Window.kbd.KeyIsPressed(VK_RIGHT) ){
            trans = glm::translate(trans, glm::vec3(1.0 * delta, 0.0, 0.0));
            glUniformMatrix4fv(mLoc, 1, GL_FALSE, glm::value_ptr(trans));

       }else if( m_Window.kbd.KeyIsPressed('W') || m_Window.kbd.KeyIsPressed(VK_UP)){
            trans = glm::translate(trans, glm::vec3(0.0, 1.0 * delta, 0.0));
            glUniformMatrix4fv(mLoc, 1, GL_FALSE, glm::value_ptr(trans));

       }
       else if( m_Window.kbd.KeyIsPressed('S') || m_Window.kbd.KeyIsPressed(VK_DOWN) ){
            trans = glm::translate(trans, glm::vec3(0.0, -1.0 * delta, 0.0));
            glUniformMatrix4fv(mLoc, 1, GL_FALSE, glm::value_ptr(trans));;
       }

    }
    void Destroy()  override{
        vbo.UnBind();
        vao.UnBind();
        LOG("Game destryed");
    }
public:
    Game() = default;
    ~Game() = default;
};


int WINAPI WinMain(HINSTANCE Instance, HINSTANCE PrevInstance, LPSTR LpCmdLine, int NcmdShow) {
    UNUSED(Instance); UNUSED(PrevInstance); UNUSED(LpCmdLine); UNUSED(NcmdShow);
    Game my_game ;
    my_game.Run();
    return 0;
}