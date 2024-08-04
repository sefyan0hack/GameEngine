#pragma once
#include "Mesh.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
struct Transform {
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
};
class GameObject {
public:
    GameObject(glm::vec3 position, Shader &program, Mesh mesh);
    ~GameObject() = default;
    void UpMatrix();
    void Render();
    void SetPosition(const glm::vec3 &pos);
    void SetScale(const glm::vec3 &Scale);
    void Rotate(const float &x, const float &y, const float &z);
    Transform GetTransform() const;
    glm::mat4 GetModleMatrix() const;

private:
    glm::mat4 Transformation(const Transform &t);

private:
    Transform transform;
    glm::mat4 ModleMat;

    Shader* program;
    Mesh mesh;
};