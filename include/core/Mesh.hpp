#pragma once
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <core/Shader.hpp>
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
    Mesh(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices);
    ~Mesh();
public:
    auto Draw(const Material &matt, size_t InstanceCount = 1)   -> void ;
public:
    unsigned int VAO, VBO, EBO;
    GLuint vInSize;
};