#include "Camera.hpp"
#include "Log.hpp"
#include <input/Mouse.hpp>
#include <graphics/OpenGL.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/fast_trigonometry.hpp>

Camera::Camera() noexcept
    : m_Yaw(-90.0f), m_Pitch(0.0f)
    , m_FOV(45.0f)
    , m_Near(0.1f), m_Far(1000.0f)
    , m_AspectRatio(16.0f / 9.0f)
    , m_Forward(0.0f, 0.0f, -1.0f)
    , m_Up(WORLD_UP)
    , m_Right(1.0f, 0.0f, 0.0f)
    , m_Position(0.0f, 1.0f, 4.0f)
    , m_Projection(PERS ? perspective() : orthographic())
    , m_View(glm::lookAt(m_Position, m_Position + m_Forward, m_Up))
{}


auto Camera::view() const -> glm::mat4
{
    return m_View;
}


auto Camera::projection() const -> glm::mat4 {
    return m_Projection;
}

auto Camera::perspective() const -> glm::mat4
{
    return glm::perspective(
        glm::radians(m_FOV),
        m_AspectRatio,
        m_Near,
        m_Far
    );
}

auto Camera::orthographic() const -> glm::mat4
{
    // Calculate the extents based on orthographic “size” (height) and aspect ratio
    auto OrthoSize = 10.0f;
    float orthoHeight = OrthoSize;
    float orthoWidth  = OrthoSize * m_AspectRatio;

    // Build left/right/top/bottom around the origin
    float left   = -orthoWidth  * 0.5f;
    float right  =  orthoWidth  * 0.5f;
    float bottom = -orthoHeight * 0.5f;
    float top    =  orthoHeight * 0.5f;

    return glm::ortho(left, right, bottom, top, m_Near, m_Far);
}

auto Camera::move(const glm::vec3 &delta) noexcept -> void
{ 
    m_Position
        += m_Right * delta.x
        + m_Up    * delta.y
        + m_Forward * delta.z;

    m_View = glm::lookAt(m_Position, m_Position + m_Forward, m_Up);
}


auto Camera::update_vectors() -> void
{
    const float yawRad = glm::radians(m_Yaw);
    const float pitchRad = glm::radians(m_Pitch);
    
    m_Forward = glm::normalize(glm::vec3{
        std::cos(yawRad) * std::cos(pitchRad),
        std::sin(pitchRad),
        std::sin(yawRad) * std::cos(pitchRad)
    });

    // Re-calculate right and up vectors
    m_Right = glm::normalize(glm::cross(m_Forward, WORLD_UP));
    m_Up = glm::normalize(glm::cross(m_Right, m_Forward));
    m_View = glm::lookAt(m_Position, m_Position + m_Forward, m_Up);
}

auto Camera::process_mouse_movement(float xoffset, float yoffset) -> void
{
    m_Yaw += xoffset;
    m_Pitch += yoffset;

    // Always clamp pitch to avoid gimbal lock
    m_Pitch = std::clamp(m_Pitch, -MAX_SAFE_PITCH, MAX_SAFE_PITCH);

    // Normalize yaw to 0°, 360° for stability
    m_Yaw = std::fmod(m_Yaw, 360.0f);
    if (m_Yaw < 0.0f) m_Yaw += 360.0f;

    update_vectors();
}

auto Camera::set_fov(float fov) -> void { m_FOV = fov; }
auto Camera::set_aspect_ratio(float aspect) -> void { m_AspectRatio = aspect; }
auto Camera::set_clipping(float nearValue, float farValue) -> void { m_Near = nearValue; m_Far = farValue; }


auto Camera::fov() const          -> float { return m_FOV; }
auto Camera::clipping() const     -> std::pair<float, float> { return {m_Near, m_Far}; }
auto Camera::aspect_ratio() const -> float { return m_AspectRatio; }

auto Camera::position() const -> glm::vec3 { return m_Position; }
auto Camera::forward() const -> glm::vec3 { return m_Forward; }
auto Camera::up() const    -> glm::vec3 { return m_Up; }
auto Camera::right() const -> glm::vec3 { return m_Right; }
