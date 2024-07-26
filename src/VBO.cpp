#include <glad/glad.h>
#include "VBO.hpp"

VBO::VBO(GLfloat *vertices, GLuint size):Id(0) {
    glGenBuffers(1, &Id);
    Bind();
    UpData(vertices, size);
}

void VBO::UpData(GLfloat *vertices, GLuint size){
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
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