#pragma once
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <core/gl.h>

struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

class Material;

class Mesh
{
public:
    Mesh(const std::vector<Vertex> &vertices, const std::vector<GLuint> &indices);
    ~Mesh();
public:
    GLuint VAO, VBO, EBO;
    GLuint vInSize;
};