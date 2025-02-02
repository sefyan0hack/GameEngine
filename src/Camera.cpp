#include <core/Camera.hpp>
#include <core/Window.hpp>
#include <core/Log.hpp>
#include <core/gl.h>
#include <algorithm>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/fast_trigonometry.hpp>

Camera::Camera(Window &window) 
: Position({0, 2, 0}),
  FrontDir({0, 0, -1}), 
  UpDir({ 0, 1, 0}),
  RightDir({1, 0, 0}),
  m_Window(&window),
  sensitivity(0.11f),
  yaw(-90), pitch(0)
  {
    Log::Info("{}", *this);
  }

Camera::~Camera() {
}

auto Camera::GetView() const -> glm::mat4
{
    return glm::lookAt(Position, Position + FrontDir, UpDir);
}


auto Camera::GetPerspective() const ->  glm::mat4
{
    auto height = m_Window->GetHeight();

    auto aspect_ = height > 0 ? static_cast<float>(m_Window->GetWidth()) / static_cast<float>(height) : 1.0f;
    auto fov_ = glm::radians(45.0f);
    auto near_ = 0.1f;
    auto far_ = 100.0f;
    return glm::perspective(fov_, aspect_, near_, far_);
}

auto Camera::MoveFroward(float speed) -> void
{
    //negate Z 
    Position -= (-FrontDir * speed);
    
}

auto Camera::MoveBackward(float speed) -> void
{
    //negate Z 
    Position += (-FrontDir * speed);
}

auto Camera::MoveUP(float speed) -> void
{
    Position += (UpDir  * speed);
}
auto Camera::MoveDown(float speed) -> void
{
    Position -= (UpDir  * speed);
}

auto Camera::MoveRight(float speed) -> void
{
    auto c = glm::cross(FrontDir, UpDir);
    Position += (c  * speed);
}

auto Camera::MoveLeft(float speed) -> void
{
    auto c = glm::cross(FrontDir, UpDir);
    Position -= (c  * speed);
}

auto Camera::EnableMSAA() -> void
{
    glEnable(GL_MULTISAMPLE);
}

auto Camera::UpdateVectors() -> void
{
    auto cosyaw  = cos(glm::radians(this->yaw));
    auto sinyaw  = sin(glm::radians(this->yaw));

    auto cospich = cos(glm::radians(this->pitch));
    auto sinpich = sin(glm::radians(this->pitch));
    glm::vec3 front;

    front = {cosyaw * cospich, sinpich, sinyaw * cospich};
    
    this->FrontDir = glm::normalize(front);
    this->RightDir = glm::normalize(glm::cross(this->FrontDir, {0, 1, 0}));
    this->UpDir = glm::normalize(glm::cross(this->RightDir, this->FrontDir));
}

auto Camera::MoseMove(bool islocked) -> void
{
    constexpr float LIMIT_ANGLE = 45.0f;
    
    while (auto op = m_Window->mouse.ReadRawDelta()) {
        float xoff = static_cast<float>(op->x)* sensitivity;
        float yoff = static_cast<float>(-op->y) * sensitivity;
        this->yaw += xoff;
        this->pitch += yoff;
    }

    if(islocked){
        this->pitch = std::clamp(this->pitch, -LIMIT_ANGLE, LIMIT_ANGLE);
    }

    //move this code later 
    static auto on = false;
    static auto lastState = false;

    auto currentState = m_Window->kbd.KeyIsPressed('L');
    if (currentState && !lastState) {
        on = !on;
    }
    lastState = currentState;

    if(on){
        m_Window->mouse.SetPos(m_Window->GetWidth()/2, m_Window->GetHeight()/2);
        ShowCursor(false);
    }else{
        ShowCursor(true);
    }
    //fin

    UpdateVectors();
}

auto Camera::SetFrontVector(glm::vec3 front)  -> void { FrontDir = front; }
auto Camera::SetUpVector(glm::vec3 up)        -> void { FrontDir = up; }
auto Camera::SetRightVector(glm::vec3 right)  -> void { RightDir = right; }

auto Camera::GetPosition() const -> glm::vec3 { return Position; }
auto Camera::GetFrontDir() const -> glm::vec3 { return FrontDir; }
auto Camera::GetUpDir() const    -> glm::vec3 { return UpDir; }
auto Camera::GetRightDir() const -> glm::vec3 { return RightDir; }
auto Camera::GetSensitivity() const -> float  { return sensitivity; }