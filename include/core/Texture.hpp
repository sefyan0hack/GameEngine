#pragma once
#include <unordered_map>
#include <string>
#include <format>
#include <core/gl.h>

class Texture
{
public:
    Texture(const std::string &name, const GLenum  Type = GL_TEXTURE_2D);
    ~Texture();
public:
    // auto Load(const std::string &name, const GLenum  Type = GL_TEXTURE_2D) -> void ;
    auto Getid() const -> GLuint ;
    auto Bind() const -> void ;
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
    GLubyte* data;
    GLboolean mipmapped;
};


template<>
struct std::formatter<Texture> {
  constexpr auto parse(std::format_parse_context& context) {
    return context.begin();
  }
  auto format(const Texture& obj, std::format_context& context) const {
    return std::format_to(context.out(), "Texture: {{ id: {}, width: {}, height: {}, type: {}, mipmapped: {} }}", obj.Getid(), obj.GetWidth(), obj.GetHeight(), obj.GetTypeName(), obj.isMipMapped());
  }
  auto format(Texture* obj, std::format_context& context) const {
    return std::format_to(context.out(), "Texture: {{ id: {}, width: {}, height: {}, type: {}, mipmapped: {} }}", obj->Getid(), obj->GetWidth(), obj->GetHeight(), obj->GetTypeName(), obj->isMipMapped());
  }
};

template<>
struct std::formatter<Texture*> {
  constexpr auto parse(std::format_parse_context& context) {
    return context.begin();
  }
  auto format(const Texture* obj, std::format_context& context) const {
    return std::format_to(context.out(), "Texture: {{ id: {}, width: {}, height: {}, type: {}, mipmapped: {} }}", obj->Getid(), obj->GetWidth(), obj->GetHeight(), obj->GetTypeName(), obj->isMipMapped());
  }
};

namespace {
    inline static std::unordered_map<GLenum, std::string> TEXTURETYPE {
        { GL_TEXTURE_2D, "GL_TEXTURE_2D"},
        { GL_PROXY_TEXTURE_2D, "GL_PROXY_TEXTURE_2D"},
        { GL_TEXTURE_1D_ARRAY, "GL_TEXTURE_1D_ARRAY"},
        { GL_PROXY_TEXTURE_1D_ARRAY, "GL_PROXY_TEXTURE_1D_ARRAY"},
        { GL_TEXTURE_RECTANGLE, "GL_TEXTURE_RECTANGLE"},
        { GL_PROXY_TEXTURE_RECTANGLE, "GL_PROXY_TEXTURE_RECTANGLE"},
        { GL_TEXTURE_CUBE_MAP_POSITIVE_X, "GL_TEXTURE_CUBE_MAP_POSITIVE_X"},
        { GL_TEXTURE_CUBE_MAP_NEGATIVE_X, "GL_TEXTURE_CUBE_MAP_NEGATIVE_X"},
        { GL_TEXTURE_CUBE_MAP_POSITIVE_Y, "GL_TEXTURE_CUBE_MAP_POSITIVE_Y"},
        { GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, "GL_TEXTURE_CUBE_MAP_NEGATIVE_Y"},
        { GL_TEXTURE_CUBE_MAP_POSITIVE_Z, "GL_TEXTURE_CUBE_MAP_POSITIVE_Z"},
        { GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, "GL_TEXTURE_CUBE_MAP_NEGATIVE_Z"},
        { GL_PROXY_TEXTURE_CUBE_MAP, "GL_PROXY_TEXTURE_CUBE_MA"},
    };
}