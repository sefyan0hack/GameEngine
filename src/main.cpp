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
    std::vector<Vertex> vertices {
        Vertex{ {-0.25, -0.25, -0.25 }/*, { 1.0f, 0.0f, 1.0f } */ }, // Vertex 1
        Vertex{ {0.25, -0.25, -0.25  }/*, { 1.0f, 0.0f, 1.0f } */ }, // Vertex 2
        Vertex{ {-0.25, 0.25, -0.25  }/*, { 1.0f, 0.0f, 1.0f } */ }, // Vertex 3
        Vertex{ {0.25, 0.25, -0.25   }/*, { 1.0f, 0.0f, 0.0f } */ }, // Vertex 4
        Vertex{ {-0.25, -0.25, 0.25  }/*, { 1.0f, 0.0f, 0.0f } */ }, // Vertex 5
        Vertex{ {0.25, -0.25, 0.25   }/*, { 1.0f, 0.0f, 0.0f } */ }, // Vertex 6
        Vertex{ {-0.25, 0.25, 0.25   }/*, { 1.0f, 0.0f, 1.0f } */ }, // Vertex 7
        Vertex{ {0.25, 0.25, 0.25    }/*, { 1.0f, 0.0f, 1.0f } */ }, // Vertex 8
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

    Mesh cube;
    glm::mat4 trans;
    glm::mat4 pers ;
    GLuint umat, pLoc;
    Shader pshad;
public: // init here 
    Game()
    : cube({vertices, ebo_indices}), trans(glm::mat4(1.0f)), pshad(SHADER(Traingl))
    {
        cube.setupMesh();
        trans = glm::rotate(trans, glm::radians(10.0f), glm::vec3(Rand_float, Rand_float, Rand_float));
        glUniformMatrix4fv(umat, 1, GL_FALSE, glm::value_ptr(trans));
        umat = pshad.GetUniformLocation("u_mat");
        pLoc = pshad.GetUniformLocation("persp");
        pers = glm::perspective(glm::radians(45.0f),(float)m_Window.GetWidth()/(float)m_Window.GetHeight(), 0.1f, 10.0f);
        glUniformMatrix4fv(pLoc, 1, GL_FALSE, glm::value_ptr(pers));
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
                glUniformMatrix4fv(umat, 1, GL_FALSE, glm::value_ptr(trans));
            }
        }
            
        cube.Draw(pshad);

       if( m_Window.kbd.KeyIsPressed('A') || m_Window.kbd.KeyIsPressed(VK_LEFT)){
            trans = glm::translate(trans, glm::vec3(-1.0 * delta, 0.0, 0.0));
            glUniformMatrix4fv(umat, 1, GL_FALSE, glm::value_ptr(trans));

       }else if( m_Window.kbd.KeyIsPressed('D') || m_Window.kbd.KeyIsPressed(VK_RIGHT) ){
            trans = glm::translate(trans, glm::vec3(1.0 * delta, 0.0, 0.0));
            glUniformMatrix4fv(umat, 1, GL_FALSE, glm::value_ptr(trans));

       }else if( m_Window.kbd.KeyIsPressed('W') || m_Window.kbd.KeyIsPressed(VK_UP)){
            trans = glm::translate(trans, glm::vec3(0.0, 1.0 * delta, 0.0));
            glUniformMatrix4fv(umat, 1, GL_FALSE, glm::value_ptr(trans));

       }
       else if( m_Window.kbd.KeyIsPressed('S') || m_Window.kbd.KeyIsPressed(VK_DOWN) ){
            trans = glm::translate(trans, glm::vec3(0.0, -1.0 * delta, 0.0));
            glUniformMatrix4fv(umat, 1, GL_FALSE, glm::value_ptr(trans));

       }else if( m_Window.kbd.KeyIsPressed(VK_SPACE) ){
            trans = glm::translate(trans, glm::vec3(0.0, 0.0, -1.0 * delta ));
            glUniformMatrix4fv(umat, 1, GL_FALSE, glm::value_ptr(trans));
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