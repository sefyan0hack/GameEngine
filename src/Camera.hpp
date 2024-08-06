#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Mouse.hpp"
class Camera
{
public:
    Camera(int width, int height, std::pair<int, int> mouse);
    ~Camera();
    void UpdateMat();
    glm::mat4 GetViewMat();
    void SetViewMat(glm::mat4 mat);
    void SetFrontVector(glm::vec3 front) { FrontDir = front; }
    void SetUpVector(glm::vec3 up) { FrontDir = up; }
    void SetRightVector(glm::vec3 right) { RightDir = right; }
    void MoveFroward(float speed);
    void MoveBackward(float speed);
    void MoveUP(float speed);
    void MoveDown(float speed);
    void MoveRight(float speed);
    void MoveLeft(float speed);
    void EnableMSAA();
    glm::vec3 GetPosition() const { return Position; }
    glm::vec3 GetFrontDir() const { return FrontDir; }
    glm::vec3 GetUpDir() const { return UpDir; }
    glm::vec3 GetRightDir() const { return RightDir; }
    
    void UpdateVectors();
    void MoseMove(Mouse &mouse, bool islocked = true);

private:
    glm::vec3 Position;
    glm::vec3 FrontDir;
    glm::vec3 UpDir;
    glm::vec3 RightDir;

    glm::mat4 ViewMat;
    //
    int Width, Height;
    float sensitivity;
    float yaw, pitch;
    glm::vec2 OldMose;
};