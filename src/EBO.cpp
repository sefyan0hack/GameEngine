#include "EBO.hpp"
EBO::EBO(){
    glGenBuffers(1, &Id);
}

void EBO::Bind()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Id);
}

void EBO::UnBind()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void EBO::UpData(GLenum target, GLsizeiptr size, const void *data, GLenum usage)
{
    glBufferData(target, size, data, usage);
}
