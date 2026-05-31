#pragma once

#include <format>
#include <utility>

#include <emath/vec3.hpp>
#include <emath/mat4.hpp>

#include <engine_export.h>

class ENGINE_EXPORT Camera
{
public:
    friend struct std::formatter<Camera>;
    
    Camera() noexcept;

    Camera(const Camera& other) = default;
    auto operator=(const Camera& other) -> Camera& = default;

    Camera(Camera&& other) noexcept = default;
    auto operator=(Camera&& other) noexcept -> Camera& = default;
    
    auto move(const emath::vec3& delta) noexcept -> void ;

    auto set_fov(float fov)                 -> void ;
    auto set_aspect_ratio(float aspect)      -> void ;
    auto set_clipping(float nearValue, float farValue) -> void ; 

    auto process_mouse_movement(float xoffset, float yoffset) -> void ;
    auto perspective() const                -> emath::mat4 ;
    auto orthographic() const               -> emath::mat4 ;
  
    auto fov() const          -> float ;
    auto clipping() const     -> std::pair<float, float> ;
    auto aspect_ratio() const -> float ;

    auto position() const     -> emath::vec3 ;
    auto projection() const   -> emath::mat4 ;
    auto view() const         -> emath::mat4 ;

    auto forward() const      -> emath::vec3 ;
    auto up() const           -> emath::vec3 ;
    auto right() const        -> emath::vec3 ;

private:
    auto update_vectors()                    -> void ;
public:
    inline    static emath::vec3 WORLD_UP = { 0, 1, 0 };
    constexpr static float MAX_SAFE_PITCH = 89.0f;
    constexpr static float LOCKED_PITCH_LIMIT = 45.0f;
    constexpr static bool PERS = true; //compile time toggle fro the projection

private:
    float m_Yaw, m_Pitch;

    float m_FOV;
    float m_Near, m_Far;
    float m_AspectRatio;

    emath::vec3 m_Forward;
    emath::vec3 m_Up;
    emath::vec3 m_Right;

    emath::vec3 m_Position;
    emath::mat4 m_Projection;
    emath::mat4 m_View;

    
};

#ifdef __cpp_lib_formatters
// custom Camera Format
template<>
struct std::formatter<Camera> {
  constexpr auto parse(std::format_parse_context& context) {
    return context.begin();
  }
  auto format(const Camera& obj, auto& context) const {
    return std::format_to(context.out(),
    R"({{ "position": {}, "view": {} }})"
    , obj.m_Position, obj.view());
  }
};
#endif