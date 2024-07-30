#include <glad/glad.h>
#include "VBO.hpp"

VBO::VBO() : Id(0)
{
    glGenBuffers(1, &Id);
}

VBO::VBO(const std::vector<float> &vertices) : Id(0)
{
    glGenBuffers(1, &Id);
}

void VBO::UpData(const std::vector<float> &vertices){
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
}

VBO::~VBO(){
    glDeleteBuffers(1, &Id);
}

void VBO::Bind() const{
    glBindBuffer(GL_ARRAY_BUFFER, Id);
}

void VBO::UnBind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}