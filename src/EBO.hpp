#pragma once
#include "Global_H.hpp"

NO_WARNING_BEGIN
#include <glad/glad.h>
NO_WARNING_END

class EBO
{
public:
    EBO();
    ~EBO() = default;
public:
    void Bind();
    void UnBind();
    void UpData(GLenum target, GLsizeiptr size, const void *data, GLenum usage);
private:
    GLuint Id;
};