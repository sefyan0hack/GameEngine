#include <iostream>
#include "Window.hpp"
#include "Global_H.hpp"
#include "G_vars.hpp"
#include "Mesh.hpp"
#include "APP.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace std;

void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
class Game : public APP
{
    private:
    std::vector<float> vertices {
        -0.25, -0.25, -0.25, 1.0f, 0.0f, 1.0f, // Vertex 1
        0.25, -0.25, -0.25,  1.0f, 0.0f, 1.0f, // Vertex 2
        -0.25, 0.25, -0.25,  1.0f, 0.0f, 1.0f, // Vertex 3
        0.25, 0.25, -0.25,   1.0f, 0.0f, 0.0f, // Vertex 4
        -0.25, -0.25, 0.25,  1.0f, 0.0f, 0.0f, // Vertex 5
        0.25, -0.25, 0.25,   1.0f, 0.0f, 0.0f, // Vertex 6
        -0.25, 0.25, 0.25,   1.0f, 0.0f, 1.0f, // Vertex 7
        0.25, 0.25, 0.25    ,1.0f, 0.0f, 1.0f, // Vertex 8
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

    Mesh* cube;
    glm::mat4 trans;
public:
    void Start() override{
        trans = glm::mat4(1.0f);
        cube = new Mesh(vertices, ebo_indices, SHADER(Traingl));
        trans = glm::rotate(trans, glm::radians(10.0f), glm::vec3(Rand_float, Rand_float, Rand_float));
        // The first argument should be familiar by now which is the uniform's location. The second argument tells OpenGL how many matrices we'd like to send, which is 1. The third argument asks us if we want to transpose our matrix, that is to swap the columns and rows. OpenGL developers often use an internal matrix layout called column-major ordering which is the default matrix layout in GLM so there is no need to transpose the matrices; we can keep it at GL_FALSE. The last parameter is the actual matrix data, but GLM stores their matrices' data in a way that doesn't always match OpenGL's expectations so we first convert the data with GLM's built-in function value_ptr.
        glUniformMatrix4fv(cube->Getmat4Loc(), 1, GL_FALSE, glm::value_ptr(trans));
        cube->Bind();
        LOG("Game started");
        m_Window.kbd.EnableAutorepeat();
    }
    void Update(float delta) override{
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
                glUniformMatrix4fv(cube->Getmat4Loc(), 1, GL_FALSE, glm::value_ptr(trans));
            }
        }
            
        glDrawElements(GL_TRIANGLES, ebo_indices.size(), GL_UNSIGNED_INT, 0);

       if( m_Window.kbd.KeyIsPressed('A') || m_Window.kbd.KeyIsPressed(VK_LEFT)){
            trans = glm::translate(trans, glm::vec3(-1.0 * delta, 0.0, 0.0));
            glUniformMatrix4fv(cube->Getmat4Loc(), 1, GL_FALSE, glm::value_ptr(trans));

       }else if( m_Window.kbd.KeyIsPressed('D') || m_Window.kbd.KeyIsPressed(VK_RIGHT) ){
            trans = glm::translate(trans, glm::vec3(1.0 * delta, 0.0, 0.0));
            glUniformMatrix4fv(cube->Getmat4Loc(), 1, GL_FALSE, glm::value_ptr(trans));

       }else if( m_Window.kbd.KeyIsPressed('W') || m_Window.kbd.KeyIsPressed(VK_UP)){
            trans = glm::translate(trans, glm::vec3(0.0, 1.0 * delta, 0.0));
            glUniformMatrix4fv(cube->Getmat4Loc(), 1, GL_FALSE, glm::value_ptr(trans));

       }
       else if( m_Window.kbd.KeyIsPressed('S') || m_Window.kbd.KeyIsPressed(VK_DOWN) ){
            trans = glm::translate(trans, glm::vec3(0.0, -1.0 * delta, 0.0));
            glUniformMatrix4fv(cube->Getmat4Loc(), 1, GL_FALSE, glm::value_ptr(trans));
       }
    }
    void Destroy()  override{
        delete cube;
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