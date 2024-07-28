#include <iostream>
#include <windows.h>
#include "Window.hpp"
#include "Global_H.hpp"
#include "G_vars.hpp"
#include "VAO.hpp"
#include "VBO.hpp"
#include "Shader.hpp"
#include "APP.hpp"
using namespace std;

class Game : public APP
{
    private:
    std::vector<float> vertices {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };

    VAO vao;
    VBO vbo;
    Shader Tring;
public:
    void Start() override{
        vao.Bind(); // 1 - bind vao
        vbo.Bind(); // 2 - bind vbo
        vbo.UpData(vertices); // 3 - up data to vbo
        vao.SetLout(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); // 4 - set attribs
        // vbo.UnBind(); // 5 - unbind if you want  vbo  (not vao)
        Tring.Load(SHADER(Traingl));
        LOG("Game started");
        Tring.UseProgram();
    }
    void Update(float delta) override{
        glDrawArrays(GL_TRIANGLES, 0, 3);
       if( m_Window.kbd.KeyIsPressed('A') ){
            vertices[0] -= delta;
            vertices[3] -= delta;
            vertices[6] -= delta;
            vbo.UpData(vertices);
       }else if( m_Window.kbd.KeyIsPressed('D') ){
            vertices[0] += delta;
            vertices[3] += delta;
            vertices[6] += delta;
            vbo.UpData(vertices);
       }else if( m_Window.kbd.KeyIsPressed('W') ){
            vertices[1] += delta;
            vertices[4] += delta;
            vertices[7] += delta;
            vbo.UpData(vertices);
       }
       else if( m_Window.kbd.KeyIsPressed('S') ){
            vertices[1] -= delta;
            vertices[4] -= delta;
            vertices[7] -= delta;
            vbo.UpData(vertices);
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