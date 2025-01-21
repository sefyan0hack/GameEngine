#include <core/Camera.hpp>
#include <core/Window.hpp>
#include <core/Shader.hpp>
#include <core/Global_H.hpp>
#include <iostream>
#include <cmath>
#include <ctime>
#include <glad/glad.h>

NO_WARNING_END

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/fast_trigonometry.hpp>

Camera::Camera(Window &window, Shader& shader) 
: Position({0, 2, 0}),
  FrontDir({0, 0, -1}), 
  UpDir({ 0, 1, 0}),
  RightDir({1, 0, 0}),
  ViewMat(glm::lookAt(Position, Position + FrontDir, UpDir)),
  m_Window(&window),
  sensitivity(0.11f),
  yaw(-90), pitch(0),
  ProgramShader(&shader){}

Camera::~Camera() {}

auto Camera::UpdateMat() -> void
{
    ViewMat = glm::lookAt(Position, Position + FrontDir, UpDir);
}

auto Camera::UpdateView() -> void
{
    ProgramShader->SetUniform("Camera", ViewMat);
}
auto Camera::UpdatePersp() -> void
{
    auto height = m_Window->GetHeight();

    auto aspect_ = height > 0 ? static_cast<float>(m_Window->GetWidth()) / height : 1.0f;
    auto fov_ = glm::radians(45.0f);
    auto near_ = 0.1f;
    auto far_ = 100.0f;
    PerspectiveMat = glm::perspective(fov_, aspect_, near_, far_);
    ProgramShader->SetUniform("Perspective", PerspectiveMat);
}
// get const ref to View Matrix
auto Camera::GetViewMat() const -> glm::mat4 const &
{
    return ViewMat;
}

auto Camera::SetViewMat(const glm::mat4 &mat) -> void
{
    ViewMat = mat;
    glm::mat4 invView = glm::inverse(mat);
    FrontDir = glm::normalize(-glm::vec3(invView[2]));
    //dont change its up of the wold space
    // UpDir =  glm::normalize(glm::vec3(invView[1]));
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
    UpdateView();
}

auto Camera::MoveUP(float speed) -> void
{
    Position += (UpDir  * speed);
    UpdateView();
}
auto Camera::MoveDown(float speed) -> void
{
    Position -= (UpDir  * speed);
    UpdateView();
}

auto Camera::MoveRight(float speed) -> void
{
    auto c = glm::cross(FrontDir, UpDir);
    Position += (c  * speed);
    UpdateView();
}

auto Camera::MoveLeft(float speed) -> void
{
    auto c = glm::cross(FrontDir, UpDir);
    Position -= (c  * speed);
    UpdateView();
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

    glm::vec3 front {0,0,0};
    front.x = cosyaw * cospich;
    front.y = sinpich;
    front.z = sinyaw * cospich;
    this->FrontDir = glm::normalize(front);
    this->RightDir = glm::normalize(glm::cross(this->FrontDir, {0, 1, 0}));
    this->UpDir = glm::normalize(glm::cross(this->RightDir, this->FrontDir));
}

auto Camera::MoseMove(bool islocked) -> void
{
    constexpr float LIMIT_ANGLE = 45.0f;
    
    while (auto op = m_Window->mouse.ReadRawDelta()) {
        float xoff = op->x * sensitivity;
        float yoff = -op->y * sensitivity;
        this->yaw += xoff;
        this->pitch += yoff;
    }

    if(islocked){
        this->pitch = std::clamp(this->pitch, -LIMIT_ANGLE, LIMIT_ANGLE);
    }

    UpdateVectors();
    UpdateMat();
    UpdatePersp();
    UpdateView();

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
}

auto Camera::SetFrontVector(glm::vec3 front)  -> void { FrontDir = front; }
auto Camera::SetUpVector(glm::vec3 up)        -> void { FrontDir = up; }
auto Camera::SetRightVector(glm::vec3 right)  -> void { RightDir = right; }

auto Camera::GetPosition() const -> glm::vec3 { return Position; }
auto Camera::GetFrontDir() const -> glm::vec3 { return FrontDir; }
auto Camera::GetUpDir() const    -> glm::vec3 { return UpDir; }
auto Camera::GetRightDir() const -> glm::vec3 { return RightDir; }

auto Camera::GetShader() const -> const Shader &
{
    if(ProgramShader)
        return *ProgramShader;
    
    ERR("Shader is null");
}