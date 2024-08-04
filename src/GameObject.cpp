#include "GameObject.hpp"

GameObject::GameObject(glm::vec3 position, Shader &program, Mesh mesh)
: transform({position, {0,0,0}, {1,1,1}}), ModleMat(1.0f), program(&program), mesh(mesh)
{   
    ModleMat = Transformation(transform);
}

void GameObject::UpMatrix()
{
    program->SetUniform("Modle", ModleMat);
}

void GameObject::Render()
{
    UpMatrix();
    mesh.Draw(*program);
}

Transform GameObject::GetTransform() const
{
    return transform;
}

glm::mat4 GameObject::GetModleMatrix() const
{
    return ModleMat;
}

void GameObject::SetPosition(const glm::vec3 &pos)
{
    transform.position = pos;
    ModleMat = glm::translate(ModleMat, transform.position);
}

void GameObject::SetScale(const glm::vec3 &Scale)
{
    transform.scale = Scale;
    ModleMat = glm::scale(ModleMat, transform.scale);
}

void GameObject::Rotate(const float &x, const float &y, const float &z)
{
    transform.rotation = {x, y, z};
    ModleMat = glm::rotate(ModleMat, glm::radians(x), glm::vec3(1, 0, 0));
    ModleMat = glm::rotate(ModleMat, glm::radians(y), glm::vec3(0, 1, 0));
    ModleMat = glm::rotate(ModleMat, glm::radians(z), glm::vec3(0, 0, 1));
}

glm::mat4 GameObject::Transformation(const Transform &t)
{
    glm::mat4 transformation = glm::mat4(1.0f);
    // Apply translation
    transformation = glm::translate(transformation, t.position);
    
    // Apply rotation (in radians)
    transformation = glm::rotate(transformation, glm::radians(t.rotation.x), glm::vec3(1, 0, 0)); // Rotate around X
    transformation = glm::rotate(transformation, glm::radians(t.rotation.y), glm::vec3(0, 1, 0)); // Rotate around Y
    transformation = glm::rotate(transformation, glm::radians(t.rotation.z), glm::vec3(0, 0, 1)); // Rotate around Z
    
    // Apply scaling
    transformation = glm::scale(transformation, t.scale);
    return transformation;
}
