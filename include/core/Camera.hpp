#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <core/fmts.hpp>
#include <format>
#include <memory>

class Window;
class Material;

class Camera
{
public:
    friend struct std::formatter<Camera>;
    
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
    auto GetView() const                       -> glm::mat4 ;
    auto GetPerspective() const                -> glm::mat4 ;
    auto GetPosition() const              -> glm::vec3 ;
    auto GetFrontDir() const              -> glm::vec3 ;
    auto GetUpDir() const                 -> glm::vec3 ;
    auto GetRightDir() const              -> glm::vec3 ;
    auto GetSensitivity() const              -> float ;

private:
    glm::vec3 Position;
    glm::vec3 FrontDir;
    glm::vec3 UpDir;
    glm::vec3 RightDir;

    //
    std::shared_ptr<Window> m_Window;
    float sensitivity;
    float yaw, pitch;
};

// custom Camera Format
template<>
struct std::formatter<Camera> {
  constexpr auto parse(std::format_parse_context& context) {
    return context.begin();
  }
  auto format(const Camera& obj, std::format_context& context) const {
    return std::format_to(context.out(),
    "Camera: {{ position: {}, sensitivity: {}, view: {} }}"
    , obj.Position, obj.sensitivity, obj.GetView());
  }
};