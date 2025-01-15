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
    auto UpdateMat()                      -> void ;
    auto UpdateView()                     -> void ;
    auto UpdatePersp()                    -> void ;
    auto SetViewMat(const glm::mat4 &mat) -> void ;
    auto SetFrontVector(glm::vec3 front)  -> void ;
    auto SetUpVector(glm::vec3 up)        -> void ;
    auto SetRightVector(glm::vec3 right)  -> void ;
    auto MoveFroward(float speed)         -> void ;
    auto MoveBackward(float speed)        -> void ;
    auto MoveUP(float speed)              -> void ;
    auto MoveDown(float speed)            -> void ;
    auto MoveRight(float speed)           -> void ;
    auto MoveLeft(float speed)            -> void ;
    auto EnableMSAA()                     -> void ;
    auto UpdateVectors()                  -> void ;
    auto MoseMove(bool islocked = true)   -> void ;
    auto GetPosition() const              -> glm::vec3 ;
    auto GetFrontDir() const              -> glm::vec3 ;
    auto GetUpDir() const                 -> glm::vec3 ;
    auto GetRightDir() const              -> glm::vec3 ;
    auto GetViewMat() const               -> glm::mat4 const & ;

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