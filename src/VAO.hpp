#pragma once

class VAO
{
public:
    VAO();
    ~VAO();
public:
    void Bind() const;
    void UnBind() const;
    void SetLout(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer);
private:
    GLuint Id;
};