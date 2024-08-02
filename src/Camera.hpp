#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera
{
public:
    Camera();
    ~Camera();
    void UpdateMat();
    glm::mat4 GetViewMat();
    void SetViewMat(glm::mat4 mat);
    void MoveFroward(float speed);
    void MoveBackward(float speed);
    void MoveUP(float speed);
    void MoveDown(float speed);
    void MoveRight(float speed);
    void MoveLeft(float speed);
    void EnableMSAA();

private:
    glm::vec3 Position;
    glm::vec3 FrontDir;
    glm::vec3 UpDir;

    glm::mat4 ViewMat;
};