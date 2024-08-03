#include "Camera.hpp"
#include <iostream>
#include <cmath>
#include <ctime>
#include "Global_H.hpp"
NO_WARNING_BEGIN
#include <glad/glad.h>
NO_WARNING_END

Camera::Camera(int width, int height) 
: Position({0.5, 0, 6}),
  FrontDir({0, 0, -1}), 
  UpDir({ 0, 1, 0}),
  ViewMat(glm::lookAt(Position, Position + FrontDir, UpDir)), 
  Width(width), Height(height){}
Camera::~Camera() {}

void Camera::UpdateMat()
{
    ViewMat = glm::lookAt(Position, Position + FrontDir, UpDir);
}

glm::mat4 Camera::GetViewMat()
{
    UpdateMat();
    return ViewMat;
}

void Camera::SetViewMat(glm::mat4 mat)
{
    ViewMat = mat;
    glm::mat4 invView = glm::inverse(mat);
    FrontDir = glm::normalize(-glm::vec3(invView[2]));
    //dont change its up of the wold space
    // UpDir =  glm::normalize(glm::vec3(invView[1]));
}

void Camera::MoveFroward(float speed)
{
    //negate Z 
    Position -= (-FrontDir * speed);
    std::cout << "X : " << Position.x << "\n"
              << "Y : " << Position.y << "\n"
              << "Z : " << Position.z << "\n";
}

void Camera::MoveBackward(float speed)
{
    //negate Z 
    Position += (-FrontDir * speed);
    std::cout << "X : " << Position.x << "\n"
              << "Y : " << Position.y << "\n"
              << "Z : " << Position.z << "\n";
}

void Camera::MoveUP(float speed)
{
    Position += (UpDir  * speed);
    std::cout << "X : " << Position.x << "\n"
              << "Y : " << Position.y << "\n"
              << "Z : " << Position.z << "\n";
}
void Camera::MoveDown(float speed)
{
    Position -= (UpDir  * speed);
    std::cout << "X : " << Position.x << "\n"
              << "Y : " << Position.y << "\n"
              << "Z : " << Position.z << "\n";
}

void Camera::MoveRight(float speed)
{
    auto c = glm::cross(FrontDir, UpDir);
    Position += (c  * speed);
    std::cout << "X : " << Position.x << "\n"
              << "Y : " << Position.y << "\n"
              << "Z : " << Position.z << "\n";
}

void Camera::MoveLeft(float speed)
{
    auto c = glm::cross(FrontDir, UpDir);
    Position -= (c  * speed);
    std::cout << "X : " << Position.x << "\n"
              << "Y : " << Position.y << "\n"
              << "Z : " << Position.z << "\n";
}

void Camera::EnableMSAA()
{
    glEnable(GL_MULTISAMPLE);
}