#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <core/fmts.hpp>

class CWindow;
class Material;

class Camera
{
public:
    friend struct std::formatter<Camera>;
    
    Camera();
    Camera(CWindow &window);
    ~Camera();

    auto SetFrontVector(glm::vec3 front)  -> void ;
    auto SetUpVector(glm::vec3 up)        -> void ;
    auto SetRightVector(glm::vec3 right)  -> void ;
    auto MoveFroward(float speed)         -> void ;
    auto MoveBackward(float speed)        -> void ;
    auto MoveUP(float speed)              -> void ;
    auto MoveDown(float speed)            -> void ;
    auto MoveRight(float speed)           -> void ;
    auto MoveLeft(float speed)            -> void ;
    auto UpdateVectors()                  -> void ;
    auto MoseMove(bool islocked = true)   -> void ;
    auto View() const                       -> glm::mat4 ;
    auto Perspective() const                -> glm::mat4 ;
    auto Position() const              -> glm::vec3 ;
    auto FrontDir() const              -> glm::vec3 ;
    auto UpDir() const                 -> glm::vec3 ;
    auto RightDir() const              -> glm::vec3 ;
    auto Sensitivity() const              -> float ;

private:
    glm::vec3 m_Position;
    glm::vec3 m_FrontDir;
    glm::vec3 m_UpDir;
    glm::vec3 m_RightDir;

    //
    std::shared_ptr<CWindow> m_Window;
    float m_Sensitivity;
    float m_Yaw, m_Pitch;

    FOR_TEST
};

// custom Camera Format
template<>
struct std::formatter<Camera> {
  constexpr auto parse(std::format_parse_context& context) {
    return context.begin();
  }
  auto format(const Camera& obj, std::format_context& context) const {
    return std::format_to(context.out(),
    R"({{ "position": {}, "sensitivity": {}, "view": {} }})"
    , obj.m_Position, obj.m_Sensitivity, obj.View());
  }
};