#pragma once

class VAO
{
public:
    VAO();
    ~VAO();
public:
    void Bind() const;
    void UnBind() const;
    void VerAttrib(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer);
private:
    GLuint Id;
};