#pragma once
#include <unordered_map>
#include <string>
#include <format>
#include <vector>
#include <core/gl.h>

class Texture
{
public:
    friend struct std::formatter<Texture>;
    Texture(const std::string &name, const GLenum  Type = GL_TEXTURE_2D);
    Texture(const std::vector<std::string> faces);
    ~Texture();
public:
    // auto Load(const std::string &name, const GLenum  Type = GL_TEXTURE_2D) -> void ;
    auto Getid() const -> GLuint ;
    auto Bind() const -> void ;
    auto UnBind() const -> void ;
    auto GenerateMipMap() -> void ;
    auto GetWidth() const -> GLsizei ;
    auto GetHeight() const -> GLsizei ;
    auto GetType() const -> GLenum;
    auto GetTypeName() const -> std::string;
    auto isMipMapped() const -> GLboolean;
private:
    GLuint id;
    GLenum type;
    GLsizei width, height;
    std::vector<GLubyte> data;
    GLboolean mipmapped;
};

// custom Texture Format
template<>
struct std::formatter<Texture> {
  constexpr auto parse(std::format_parse_context& context) {
    return context.begin();
  }
  auto format(const Texture& obj, std::format_context& context) const {
    return std::format_to(context.out(),
    "Texture: {{ id: {}, width: {}, height: {}, type: {}, mipmapped: {}, dataLenght: {} }}"
    , obj.id, obj.width, obj.height, obj.GetTypeName(), obj.mipmapped, obj.data.size());
  }
};

namespace {
    inline static std::unordered_map<GLenum, std::string> TEXTURETYPE {
        {GL_TEXTURE_1D, "GL_TEXTURE_1D"},
        {GL_TEXTURE_2D, "GL_TEXTURE_2D"},
        {GL_TEXTURE_3D, "GL_TEXTURE_3D"},
        {GL_TEXTURE_1D_ARRAY, "GL_TEXTURE_1D_ARRAY"},
        {GL_TEXTURE_2D_ARRAY, "GL_TEXTURE_2D_ARRAY"},
        {GL_TEXTURE_RECTANGLE, "GL_TEXTURE_RECTANGLE"},
        {GL_TEXTURE_CUBE_MAP, "GL_TEXTURE_CUBE_MAP"},
        {GL_TEXTURE_CUBE_MAP_ARRAY, "GL_TEXTURE_CUBE_MAP_ARRAY"},
        {GL_TEXTURE_BUFFER, "GL_TEXTURE_BUFFER"},
        {GL_TEXTURE_2D_MULTISAMPLE, "GL_TEXTURE_2D_MULTISAMPLE"},
        {GL_TEXTURE_2D_MULTISAMPLE_ARRAY, "GL_TEXTURE_2D_MULTISAMPLE_ARRA"},
    };
}