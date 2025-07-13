#include <core/Camera.hpp>
#include <core/Log.hpp>
#include <core/OpenGL.hpp>
#include <core/Mouse.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/fast_trigonometry.hpp>

Camera::Camera() noexcept
    : m_Position(0.0f, 1.0f, 4.0f),
      m_FrontDir(0.0f, 0.0f, -1.0f),
      m_UpDir(WORLD_UP),
      m_RightDir(1.0f, 0.0f, 0.0f),
      m_Sensitivity(0.11f),
      m_Yaw(-90.0f),
      m_Pitch(0.0f),
      m_FOV(45.0f),
      m_Near(0.1f),
      m_Far(1000.0f),
      m_AspectRatio(16.0f / 9.0f)
{}

Camera::~Camera() {
}

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

void Camera::MoveForward(float speed) noexcept { m_Position += m_FrontDir * speed; }
void Camera::MoveBackward(float speed) noexcept { m_Position -= m_FrontDir * speed; }
void Camera::MoveUp(float speed) noexcept { m_Position += m_UpDir * speed; }
void Camera::MoveDown(float speed) noexcept { m_Position -= m_UpDir * speed; }
void Camera::MoveRight(float speed) noexcept { m_Position += m_RightDir * speed; }
void Camera::MoveLeft(float speed) noexcept { m_Position -= m_RightDir * speed; }

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

auto Camera::UpdateCameraPosition(Mouse& mouse) -> void
{
    while (auto op = mouse.ReadRawDelta()) {
        auto [dx, dy] = op.value();

        float xoff = static_cast<float>(dx)* m_Sensitivity;
        float yoff = static_cast<float>(-dy) * m_Sensitivity;

        this->m_Yaw += xoff;
        this->m_Pitch += yoff;
    }

    // Always clamp pitch to avoid gimbal lock
    m_Pitch = std::clamp(m_Pitch, -MAX_SAFE_PITCH, MAX_SAFE_PITCH);

    if(mouse.Locked()){
        this->m_Pitch = std::clamp(this->m_Pitch, -LOCKED_PITCH_LIMIT, LOCKED_PITCH_LIMIT);
    }

    UpdateVectors();
}

auto Camera::SetFrontVector(glm::vec3 front)  -> void { m_FrontDir = front; }
auto Camera::SetUpVector(glm::vec3 up)        -> void { m_UpDir = up; }
auto Camera::SetRightVector(glm::vec3 right)  -> void { m_RightDir = right; }

void Camera::SetFOV(float fov) { m_FOV = fov; }

void Camera::SetClipping(float nearValue, float farValue) { m_Near = nearValue; m_Far = farValue; }

auto Camera::Position() const -> glm::vec3 { return m_Position; }
auto Camera::FrontDir() const -> glm::vec3 { return m_FrontDir; }
auto Camera::UpDir() const    -> glm::vec3 { return m_UpDir; }
auto Camera::RightDir() const -> glm::vec3 { return m_RightDir; }
auto Camera::Sensitivity() const -> float  { return m_Sensitivity; }