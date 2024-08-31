#pragma once
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include "Shader.hpp"
struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

class Mesh
{
public:
    Mesh(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices);
    ~Mesh();
public:
    void Draw(const Shader &shader, size_t InstanceCount = 1);
    void setupMesh();
public:
    unsigned int VAO, VBO, EBO;
private:
    std::vector<Vertex> Vertices;
    std::vector<unsigned int> Indices;
};