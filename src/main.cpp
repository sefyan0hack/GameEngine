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
        

public:
    void Start() override{
        LOG("Game started");

    }
    void Update(float delta) override{
        LOG(delta << m_Window.kbd.KeyIsPressed('A'));
    }
    void Destroy()  override{
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