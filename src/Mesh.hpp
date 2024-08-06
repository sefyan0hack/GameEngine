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
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices);
    ~Mesh();
public:
    void Draw(Shader &shader, size_t InstanceCount = 1);
    void setupMesh();
public:
    unsigned int VAO, VBO, EBO;
private:
    std::vector<Vertex> Vertices;
    std::vector<unsigned int> Indices;
};