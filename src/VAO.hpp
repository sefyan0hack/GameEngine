#pragma once
#include "Global_H.hpp"
NO_WARNING_BEGIN
#include <glad/glad.h>
NO_WARNING_END
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