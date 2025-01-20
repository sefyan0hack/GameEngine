#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <core/Mesh.hpp>
#include <core/Transform.hpp>
#include <vector>

class Shader;

class GameObject {
public:
    GameObject(glm::vec3 position, const Shader &program, const Mesh &mesh);
    ~GameObject();

    auto UpMatrix()                                             -> void ;
    auto SetUp(std::vector<glm::vec3> InsPos)                   -> void ;
    auto Render()                                               -> void ;
    auto SetPosition(const glm::vec3 &pos)                      -> void ;
    auto SetScale(const glm::vec3 &Scale)                       -> void ;
    auto Rotate(const float &x, const float &y, const float &z) -> void ;
    auto GetTransform() const                                   -> Transform ;
    auto GetModleMatrix() const                                 -> glm::mat4 ;
    auto GetMesh() const                                        -> const Mesh& ;
    auto GetShader() const                                        -> const Shader& ;
    auto GetInstancePos() const                                        -> const std::vector<glm::vec3>&;

private:
    auto Transformation() const                                 -> glm::mat4 ;

private:
    Transform transform;
    const Shader* program;
    Mesh m_Mesh;
    std::vector<glm::vec3> InstancePos;
};