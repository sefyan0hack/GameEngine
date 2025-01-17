#pragma once
#include <glm/glm.hpp>

class Transform
{
public:
    Transform( glm::vec3 position, glm::vec3 rotaion = {0.0f, 0.0f, 0.0f}, glm::vec3 scale = {1.0f, 1.0f, 1.0f});
    ~Transform() = default;

public:
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
};