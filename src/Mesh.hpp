#pragma once
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include "Shader.hpp"
struct Vertex
{
    glm::vec3 Position;
    // glm::vec3 Normal;
    // glm::vec2 TexCoords;
};

// struct Texture {
//     unsigned int id;
//     std::string type;
// };

class Mesh
{
public:
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices);
    ~Mesh();
public:
    void Draw(Shader &shader);
    void setupMesh();
private:
    unsigned int VAO,VBO, EBO;
    std::vector<Vertex> Vertices;
    std::vector<unsigned int> Indices;
};