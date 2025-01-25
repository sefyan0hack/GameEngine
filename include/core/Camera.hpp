#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Window;
class Material;

class Camera
{
public:
    Camera(Window &window);
    ~Camera();
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
    auto GetView()                        -> glm::mat4 ;
    auto GetPerspective()                 -> glm::mat4 ;
    auto GetPosition() const              -> glm::vec3 ;
    auto GetFrontDir() const              -> glm::vec3 ;
    auto GetUpDir() const                 -> glm::vec3 ;
    auto GetRightDir() const              -> glm::vec3 ;

private:
    glm::vec3 Position;
    glm::vec3 FrontDir;
    glm::vec3 UpDir;
    glm::vec3 RightDir;

    //
    Window* m_Window;
    float sensitivity;
    float yaw, pitch;
};