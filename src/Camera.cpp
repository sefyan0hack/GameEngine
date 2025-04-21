#include <core/Camera.hpp>
#include <core/Log.hpp>
#include <core/gl.h>

#ifdef _WIN32
#include <core/Window.hpp>
#elif defined(__linux__)
#include <core/XWindow.hpp>
#endif //_WIN32

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/fast_trigonometry.hpp>

Camera::Camera(CWindow &window) 
: m_Position({0, 2, 0}),
  m_FrontDir({0, 0, -1}), 
  m_UpDir({ 0, 1, 0}),
  m_RightDir({1, 0, 0}),
  m_Window(std::make_shared<CWindow>(window)),
  m_Sensitivity(0.11f),
  m_Yaw(-90), m_Pitch(0)
  {
    Info("{}", *this);
  }

Camera::~Camera() {
}

auto Camera::View() const -> glm::mat4
{
    return glm::lookAt(m_Position, m_Position + m_FrontDir, m_UpDir);
}


auto Camera::Perspective() const ->  glm::mat4
{
    auto height = m_Window->Height();

    auto aspect_ = height > 0 ? static_cast<float>(m_Window->Width()) / static_cast<float>(height) : 1.0f;
    auto fov_ = glm::radians(45.0f);
    auto near_ = 0.1f;
    auto far_ = 1000.0f;
    return glm::perspective(fov_, aspect_, near_, far_);
}

auto Camera::MoveFroward(float speed) -> void
{
    //negate Z 
    m_Position -= (-m_FrontDir * speed);
    
}

auto Camera::MoveBackward(float speed) -> void
{
    //negate Z 
    m_Position += (-m_FrontDir * speed);
}

auto Camera::MoveUP(float speed) -> void
{
    m_Position += (m_UpDir  * speed);
}
auto Camera::MoveDown(float speed) -> void
{
    m_Position -= (m_UpDir  * speed);
}

auto Camera::MoveRight(float speed) -> void
{
    auto c = glm::cross(m_FrontDir, m_UpDir);
    m_Position += (c  * speed);
}

auto Camera::MoveLeft(float speed) -> void
{
    auto c = glm::cross(m_FrontDir, m_UpDir);
    m_Position -= (c  * speed);
}

auto Camera::EnableMSAA() -> void
{
    glEnable(GL_MULTISAMPLE);
}

auto Camera::UpdateVectors() -> void
{
    auto cosyaw  = cos(glm::radians(this->m_Yaw));
    auto sinyaw  = sin(glm::radians(this->m_Yaw));

    auto cospich = cos(glm::radians(this->m_Pitch));
    auto sinpich = sin(glm::radians(this->m_Pitch));
    glm::vec3 front;

    front = {cosyaw * cospich, sinpich, sinyaw * cospich};
    
    this->m_FrontDir = glm::normalize(front);
    this->m_RightDir = glm::normalize(glm::cross(this->m_FrontDir, {0, 1, 0}));
    this->m_UpDir = glm::normalize(glm::cross(this->m_RightDir, this->m_FrontDir));
}

auto Camera::MoseMove(bool islocked) -> void
{
    constexpr float LIMIT_ANGLE = 45.0f;
    
    while (auto op = m_Window->m_Mouse->ReadRawDelta()) {
        float xoff = static_cast<float>(op->x)* m_Sensitivity;
        float yoff = static_cast<float>(-op->y) * m_Sensitivity;
        this->m_Yaw += xoff;
        this->m_Pitch += yoff;
    }

    if(islocked){
        this->m_Pitch = std::clamp(this->m_Pitch, -LIMIT_ANGLE, LIMIT_ANGLE);
    }

    //move this code later 
    static auto on = false;
    static auto lastState = false;

    auto currentState = m_Window->m_Keyboard->KeyIsPressed('L');
    if (currentState && !lastState) {
        on = !on;
    }
    lastState = currentState;
    #ifdef _WIN32
    if(on){
        m_Window->m_Mouse->SetPos(m_Window->Width()/2, m_Window->Height()/2);
        ShowCursor(false);
    }else{
        ShowCursor(true);
    }
    #endif //_WIN32
    //fin

    UpdateVectors();
}

auto Camera::SetFrontVector(glm::vec3 front)  -> void { m_FrontDir = front; }
auto Camera::SetUpVector(glm::vec3 up)        -> void { m_FrontDir = up; }
auto Camera::SetRightVector(glm::vec3 right)  -> void { m_RightDir = right; }

auto Camera::Position() const -> glm::vec3 { return m_Position; }
auto Camera::FrontDir() const -> glm::vec3 { return m_FrontDir; }
auto Camera::UpDir() const    -> glm::vec3 { return m_UpDir; }
auto Camera::RightDir() const -> glm::vec3 { return m_RightDir; }
auto Camera::Sensitivity() const -> float  { return m_Sensitivity; }