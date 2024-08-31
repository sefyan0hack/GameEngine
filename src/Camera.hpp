#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Window.hpp"
#include "Shader.hpp"

class Camera
{
public:
    Camera(Window &window, Shader& shader);
    ~Camera();
    void UpdateMat();
    void UpdateView();
    void UpdatePersp();
    glm::mat4 const & GetViewMat() const;
    void SetViewMat(const glm::mat4 &mat);
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
    void MoseMove(bool islocked = true);

private:
    glm::vec3 Position;
    glm::vec3 FrontDir;
    glm::vec3 UpDir;
    glm::vec3 RightDir;

    glm::mat4 ViewMat;
    glm::mat4 PerspectiveMat;
    //
    Window* m_Window;
    float sensitivity;
    float yaw, pitch;
    Shader* ProgramShader;
};