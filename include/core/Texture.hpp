#pragma once
#include <unordered_map>
#include <string>

#include <core/gl.h>
class Texture
{
public:
    Texture(const std::string &name, const GLenum  Type = GL_TEXTURE_2D);
    ~Texture() = default;
public:
    // auto Load(const std::string &name, const GLenum  Type = GL_TEXTURE_2D) -> void ;
    auto Getid() const -> GLuint ;
    auto Bind() const -> void ;
private:
    GLuint id;
    GLenum type;
};