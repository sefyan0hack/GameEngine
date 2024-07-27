#pragma once
#include "Global_H.hpp"
#include <windows.h>
#include "Renderer.hpp"
#include "VAO.hpp"
#include "VBO.hpp"
#include "Shader.hpp"

NO_WARNING_BEGIN
#include <glad/glad.h>
NO_WARNING_END

#define WM_FPS_PAINT (WM_USER + 1)

//Debug Vars
inline LPCSTR  CLASS_NAME  = "CustomClass";
inline WNDCLASSEXA windclass = 
    {
    .cbSize = sizeof(WNDCLASSEXA),
    .style = CS_GLOBALCLASS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW| CS_DBLCLKS,
    .lpfnWndProc = nullptr,
    .cbClsExtra = 0,
    .cbWndExtra = 0,
    .hInstance =  GetModuleHandleA(0),
    .hIcon = LoadIconA(nullptr, IDI_APPLICATION),
    .hCursor = LoadCursorA(nullptr, IDC_ARROW),
    .hbrBackground = nullptr,
    .lpszMenuName = "",
    .lpszClassName = CLASS_NAME,
    .hIconSm = LoadIconA(nullptr, IDI_APPLICATION),
    };
inline Window & window = Window::InstanceWindow();
inline LARGE_INTEGER start_count, end_count, counts, freq, fps;

// OpenGl Vars

inline GLfloat vertices[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };
    
inline Shader Tring(SHADER(Traingl));

inline GLint xL = glGetUniformLocation(Tring.GetProgram(), "u_offx");
inline GLint yL = glGetUniformLocation(Tring.GetProgram(), "u_offy");
inline GLint zL = glGetUniformLocation(Tring.GetProgram(), "u_offz");

inline GLfloat xyz[3] = {0.0f, 0.0f, 0.0f};

inline VAO vao;
inline VBO vbo(vertices, sizeof(vertices));
