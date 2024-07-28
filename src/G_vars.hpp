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

inline LARGE_INTEGER start_count, end_count, counts, freq, fps;
inline Window window{ 640, 480, "Main"};
inline Window window1{ 640, 480, "ain"};
inline Window window2{ 999, 100, "ain"};

// OpenGl Vars

// inline GLfloat vertices[] = {
//         -0.5f, -0.5f, 0.0f,
//          0.5f, -0.5f, 0.0f,
//          0.0f,  0.5f, 0.0f
//     };
    
// inline Shader Tring(SHADER(Traingl));

// inline GLint xL = glGetUniformLocation(Tring.GetProgram(), "u_offx");
// inline GLint yL = glGetUniformLocation(Tring.GetProgram(), "u_offy");
// inline GLint zL = glGetUniformLocation(Tring.GetProgram(), "u_offz");

// inline GLfloat xyz[3] = {0.0f, 0.0f, 0.0f};

// inline VAO vao;
// inline VBO vbo(vertices, sizeof(vertices));
