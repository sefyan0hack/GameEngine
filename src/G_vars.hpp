#pragma once
#include "Global_H.hpp"
#include <windows.h>
#include "Renderer.hpp"
#include "VAO.hpp"
#include "VBO.hpp"
#include "Shader.hpp"
#include <vector>
NO_WARNING_BEGIN
#include <glad/glad.h>
NO_WARNING_END

#define WM_FPS_PAINT (WM_USER + 1)

// OpenGl Vars

// inline  std::vector<float> vertices {
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
// inline VBO vbo(vertices);
