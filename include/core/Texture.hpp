#pragma once
#include <unordered_map>
#include <string>
#include <stb/stb_image.h>

#include <core/gl.h>
class Texture
{
public:
    Texture() = default;
    ~Texture() = default;
public:
    auto Loud(const std::string &name, const GLenum  Type = GL_TEXTURE_2D) -> void ;
    auto GetByName(const std::string &name) const -> GLuint ;
    auto BindByName(const std::string &name, const GLenum  Type = GL_TEXTURE_2D) const -> void ;
private:
    inline static std::unordered_map<std::string, GLuint> Textures;

};