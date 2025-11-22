#pragma once

#include <format>
#include <utility>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <core/fmts.hpp>
#include <engine_export.h>

class ENGINE_API Camera
{
public:
    friend struct std::formatter<Camera>;
    
    Camera() noexcept;

    Camera(const Camera& other) = default;
    auto operator=(const Camera& other) -> Camera& = default;

    Camera(Camera&& other) noexcept = default;
    auto operator=(Camera&& other) noexcept -> Camera& = default;
    
    auto move(const glm::vec3& delta) noexcept -> void ;

    auto set_fov(float fov)                 -> void ;
    auto set_aspect_ratio(float aspect)      -> void ;
    auto set_clipping(float nearValue, float farValue) -> void ; 

    auto process_mouse_movement(float xoffset, float yoffset) -> void ;
    auto perspective() const                -> glm::mat4 ;
    auto orthographic() const               -> glm::mat4 ;
  
    auto fov() const          -> float ;
    auto clipping() const     -> std::pair<float, float> ;
    auto aspect_ratio() const -> float ;

    auto position() const     -> glm::vec3 ;
    auto projection() const   -> glm::mat4 ;
    auto view() const         -> glm::mat4 ;

    auto forward() const      -> glm::vec3 ;
    auto up() const           -> glm::vec3 ;
    auto right() const        -> glm::vec3 ;

private:
    auto update_vectors()                    -> void ;
public:
    constexpr static glm::vec3 WORLD_UP = { 0, 1, 0 };
    constexpr static float MAX_SAFE_PITCH = 89.0f;
    constexpr static float LOCKED_PITCH_LIMIT = 45.0f;
    constexpr static bool PERS = true; //compile time toggle fro the projection

private:
    float m_Yaw, m_Pitch;

    float m_FOV;
    float m_Near, m_Far;
    float m_AspectRatio;

    glm::vec3 m_Forward;
    glm::vec3 m_Up;
    glm::vec3 m_Right;

    glm::vec3 m_Position;
    glm::mat4 m_Projection;
    glm::mat4 m_View;

    
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
    , obj.m_Position, obj.view());
  }
};