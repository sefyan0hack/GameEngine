#pragma once
#include <unordered_map>
#include <string>
#include <stb/stb_image.h>

#include "Global_H.hpp"
NO_WARNING_BEGIN
#include <glad/glad.h>
NO_WARNING_END
class Texture
{
public:
    Texture() = default;
    ~Texture() = default;
public:
    void Loud(const std::string &name, const int Type = GL_TEXTURE_2D);
    unsigned int GetByName(const std::string &name) const;
    void BindByName(const std::string &name, const int Type = GL_TEXTURE_2D) const;
private:
    inline static std::unordered_map<std::string, unsigned int> Textures;

};