#include <iostream>
#include <windows.h>
#include "Window.hpp"
#include "Renderer.hpp"
#include "Global_H.hpp"
#include "G_vars.hpp"
#include "VAO.hpp"
#include "VBO.hpp"
#include "Shader.hpp"
using namespace std;

int main() {
    Window & window = Window::InstanceWindow();
    VAO vao;
    VBO vbo(vertices, sizeof(vertices));

    vao.SetLout(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);

    Shader Tring(SHADER(Traingl));

    vbo.UnBind();
    vao.UnBind();

    xL = glGetUniformLocation(Tring.GetProgram(),"u_offx");
    yL = glGetUniformLocation(Tring.GetProgram(),"u_offy");
    zL = glGetUniformLocation(Tring.GetProgram(),"u_offz");
    QueryPerformanceCounter(&start_count);
    QueryPerformanceFrequency(&freq);

    while (!window.ShouldWindowClose()) {
        TextOutA(window.GetHDC(), 10, 10, std::to_string(fps.QuadPart).c_str(), 5);
        window.ProcessMessages();
        
        glClearColor(0.f, 0.3f, 0.6f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
        Tring.UseProgram();
        vao.Bind();
        glUniform1f(xL, xyz[0]);
        glUniform1f(yL, xyz[1]);
        glUniform1f(zL, xyz[2]);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        SwapBuffers(window.GetHDC());
        // Sleep(1);

        QueryPerformanceCounter(&end_count);
        counts.QuadPart = end_count.QuadPart - start_count.QuadPart;
        start_count = end_count;
        fps.QuadPart = freq.QuadPart / counts.QuadPart;
    }
    return 0;
}