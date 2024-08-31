#include "Camera.hpp"
#include <iostream>
#include <cmath>
#include <ctime>
#include "Global_H.hpp"  
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
  sensitivity(0.1),
  yaw(-90), pitch(0),
  ProgramShader(&shader){}

Camera::~Camera() {}

void Camera::UpdateMat()
{
    ViewMat = glm::lookAt(Position, Position + FrontDir, UpDir);
}

void Camera::UpdateView()
{
    ProgramShader->SetUniform("Camera", ViewMat);
}
void Camera::UpdatePersp()
{
    PerspectiveMat = glm::perspective(glm::radians(45.0f),(float)m_Window->GetWidth()/(float)m_Window->GetHeight(), 0.1f, 100.0f);
    ProgramShader->SetUniform("Perspective", PerspectiveMat);
}
// get const ref to View Matrix
glm::mat4 const & Camera::GetViewMat() const
{
    return ViewMat;
}

void Camera::SetViewMat(const glm::mat4 &mat)
{
    ViewMat = mat;
    glm::mat4 invView = glm::inverse(mat);
    FrontDir = glm::normalize(-glm::vec3(invView[2]));
    //dont change its up of the wold space
    // UpDir =  glm::normalize(glm::vec3(invView[1]));
}

void Camera::MoveFroward(float speed)
{
    //negate Z 
    Position -= (-FrontDir * speed);
    
}

void Camera::MoveBackward(float speed)
{
    //negate Z 
    Position += (-FrontDir * speed);
    UpdateView();
}

void Camera::MoveUP(float speed)
{
    Position += (UpDir  * speed);
    UpdateView();
}
void Camera::MoveDown(float speed)
{
    Position -= (UpDir  * speed);
    UpdateView();
}

void Camera::MoveRight(float speed)
{
    auto c = glm::cross(FrontDir, UpDir);
    Position += (c  * speed);
    UpdateView();
}

void Camera::MoveLeft(float speed)
{
    auto c = glm::cross(FrontDir, UpDir);
    Position -= (c  * speed);
    UpdateView();
}

void Camera::EnableMSAA()
{
    glEnable(GL_MULTISAMPLE);
}

void Camera::UpdateVectors()
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

void Camera::MoseMove(bool islocked)
{
    auto op = m_Window->mouse.ReadRawDelta();
    float xoff;
    float yoff;
    if(op){
        xoff = op.value().x;
        yoff = -op.value().y;
    }else{
        xoff = 0;
        yoff = 0;
    }
    xoff *= this->sensitivity;
    yoff *= this->sensitivity;
    this->yaw += xoff;
    this->pitch += yoff;
    if(islocked){
        if(this->pitch > 45)
            this->pitch = 45;

        if(this->pitch < -45)
            this->pitch = -45;
    }
    UpdateVectors();
    UpdateMat();
    UpdatePersp();
    UpdateView();
    // m_Window->mouse.SetPos(m_Window->GetWidth()/2.0f, m_Window->GetHeight()/2.0f);
}