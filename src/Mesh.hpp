#pragma once
#include <vector>
#include "VAO.hpp"
#include "VBO.hpp"
#include "EBO.hpp"
#include "Shader.hpp"

class Mesh
{
public:
    Mesh(std::vector<float> vertices, std::vector<GLuint> indices, std::string ShaderName);
    ~Mesh();
public:
    void Bind();
    GLuint Getmat4Loc() const;
private:
    GLuint m_PosLoc;
    VBO m_buffer;
    VAO m_specification;
    EBO m_indices;
    Shader m_program;
};