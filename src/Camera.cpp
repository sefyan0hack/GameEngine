#include <core/Camera.hpp>
#include <core/Log.hpp>
#include <core/OpenGL.hpp>
#include <core/Mouse.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/fast_trigonometry.hpp>

Camera::Camera() noexcept
    : m_Position(0.0f, 1.0f, 4.0f)
    , m_FrontDir(0.0f, 0.0f, -1.0f)
    , m_UpDir(WORLD_UP)
    , m_RightDir(1.0f, 0.0f, 0.0f)
    , m_Yaw(-90.0f), m_Pitch(0.0f)
    , m_FOV(45.0f)
    , m_Near(0.1f), m_Far(1000.0f)
    , m_AspectRatio(16.0f / 9.0f)
{}


auto Camera::View() const -> glm::mat4
{
    return glm::lookAt(m_Position, m_Position + m_FrontDir, m_UpDir);
}


auto Camera::Perspective() const ->  glm::mat4
{
    return glm::perspective(
        glm::radians(m_FOV),
        m_AspectRatio,
        m_Near,
        m_Far
    );
}

auto Camera::Move(const glm::vec3 &delta) noexcept -> void
{ 
    m_Position 
        += m_RightDir * delta.x
        + m_UpDir    * delta.y
        + m_FrontDir * delta.z;
}


auto Camera::UpdateVectors() -> void
{
    const float yawRad = glm::radians(m_Yaw);
    const float pitchRad = glm::radians(m_Pitch);
    
    m_FrontDir = glm::normalize(glm::vec3{
        std::cos(yawRad) * std::cos(pitchRad),
        std::sin(pitchRad),
        std::sin(yawRad) * std::cos(pitchRad)
    });

    // Re-calculate right and up vectors
    m_RightDir = glm::normalize(glm::cross(m_FrontDir, WORLD_UP));
    m_UpDir = glm::normalize(glm::cross(m_RightDir, m_FrontDir));
}

auto Camera::ProcessMouseMovement(float xoffset, float yoffset) -> void
{
    m_Yaw += xoffset;
    m_Pitch += yoffset;

    // Always clamp pitch to avoid gimbal lock
    m_Pitch = std::clamp(m_Pitch, -MAX_SAFE_PITCH, MAX_SAFE_PITCH);

    // Normalize yaw to 0°, 360° for stability
    m_Yaw = std::fmod(m_Yaw, 360.0f);
    if (m_Yaw < 0.0f) m_Yaw += 360.0f;

    UpdateVectors();
}

auto Camera::SetFOV(float fov) -> void { m_FOV = fov; }
auto Camera::SetAspectRatio(float aspect) -> void { m_AspectRatio = aspect; }
auto Camera::SetClipping(float nearValue, float farValue) -> void { m_Near = nearValue; m_Far = farValue; }

auto Camera::Position() const -> glm::vec3 { return m_Position; }
auto Camera::FrontDir() const -> glm::vec3 { return m_FrontDir; }
auto Camera::UpDir() const    -> glm::vec3 { return m_UpDir; }
auto Camera::RightDir() const -> glm::vec3 { return m_RightDir; }
