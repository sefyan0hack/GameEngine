#pragma once
#include "Mesh.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Transform.hpp"

class Shader;

class GameObject {
public:
    GameObject(glm::vec3 position, const Shader &program, const Mesh &mesh);
    ~GameObject();

    auto UpMatrix()                                             -> void ;
    auto SetUp(const std::vector<glm::vec3> &InsPos)            -> void ;
    auto SetUp(glm::vec3 * InsPos, size_t size)                 -> void ;
    auto Render()                                               -> void ;
    auto SetPosition(const glm::vec3 &pos)                      -> void ;
    auto SetScale(const glm::vec3 &Scale)                       -> void ;
    auto Rotate(const float &x, const float &y, const float &z) -> void ;
    auto GetTransform() const                                   -> Transform ;
    auto GetModleMatrix() const                                 -> glm::mat4 ;

private:
    auto Transformation() const                                 -> glm::mat4 ;

private:
    Transform transform;
    const Shader* program;
    Mesh m_Mesh;
    size_t InstanceCount;
    const glm::vec3* InstancePos;
};