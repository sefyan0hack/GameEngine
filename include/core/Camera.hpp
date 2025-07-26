#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <core/fmts.hpp>

class Camera
{
public:
    friend struct std::formatter<Camera>;
    
    Camera() noexcept;
    ~Camera() = default;

    Camera(const Camera& other) = default;
    auto operator=(const Camera& other) -> Camera& = default;

    Camera(Camera&& other) noexcept = default;
    auto operator=(Camera&& other) noexcept -> Camera& = default;
    
    auto Move(const glm::vec3& delta) noexcept -> void ;

    auto SetFOV(float fov)                 -> void ;
    auto SetAspectRatio(float aspect)      -> void ;
    auto SetClipping(float nearValue, float farValue) -> void ; 

    auto ProcessMouseMovement(float xoffset, float yoffset) -> void ;
    auto View() const                       -> glm::mat4 ;
    auto Perspective() const                -> glm::mat4 ;
    auto Position() const              -> glm::vec3 ;
    auto FrontDir() const              -> glm::vec3 ;
    auto UpDir() const                 -> glm::vec3 ;
    auto RightDir() const              -> glm::vec3 ;

private:
    auto UpdateVectors()                    -> void ;
public:
    constexpr static glm::vec3 WORLD_UP = { 0, 1, 0 };
    constexpr static float MAX_SAFE_PITCH = 89.0f;
    constexpr static float LOCKED_PITCH_LIMIT = 45.0f;

private:
    glm::vec3 m_Position;
    glm::vec3 m_FrontDir;
    glm::vec3 m_UpDir;
    glm::vec3 m_RightDir;

    float m_Yaw, m_Pitch;

    float m_FOV;
    float m_Near, m_Far;
    float m_AspectRatio;

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
    R"({{ "position": {}, "view": {} }})"
    , obj.m_Position, obj.View());
  }
};