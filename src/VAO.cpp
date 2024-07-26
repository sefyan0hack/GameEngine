#include <glad/glad.h>
#include "VAO.hpp"

VAO::VAO():Id(0) {
    glGenVertexArrays(1, &Id);
    Bind();
}
VAO::~VAO(){
    glDeleteVertexArrays(1, &Id);
}
void VAO::VerAttrib(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer){
    glEnableVertexAttribArray(index);
    Bind();
    glVertexAttribPointer(index, size, type, normalized, stride, pointer);
}
void VAO::Bind() const{
    glBindVertexArray(Id);
}
void VAO::UnBind() const{
    glBindVertexArray(0);
}