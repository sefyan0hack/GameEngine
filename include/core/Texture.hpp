#pragma once
#include <unordered_map>
#include <string>
#include <format>
#include <vector>
#include <array>
#include <core/gl.h>

class Texture
{
public:
    friend struct std::formatter<Texture>;
    Texture(GLenum texType);
    virtual ~Texture();
public:
    virtual auto Getid() const -> GLuint ;
    virtual auto Bind() const -> void ;
    virtual auto UnBind() const -> void ;
    virtual auto GetWidth() const -> GLsizei ;
    virtual auto GetHeight() const -> GLsizei ;
    virtual auto GetType() const -> GLenum;
    virtual auto GetTypeName() const -> std::string;
protected:
    GLuint id;
    GLenum type;
    GLsizei width, height;
};

class Texture2D : public Texture
{
  public:
    Texture2D(const std::string &name);
    auto GenerateMipMap() -> void ;
    auto isMipMapped() const -> GLboolean;
  private:
    std::vector<GLubyte> data;
    GLboolean mipmapped;
};

class TextureCubeMap : public Texture
{
  public:
    TextureCubeMap(const std::vector<std::string> faces);
  private:
    std::array<std::vector<GLubyte>, 6> data;
};

// custom Texture Format
template<>
struct std::formatter<Texture> {
  constexpr auto parse(std::format_parse_context& context) {
    return context.begin();
  }
  auto format(const Texture& obj, std::format_context& context) const {
    return std::format_to(context.out(),
    "Texture: {{ id: {}, width: {}, height: {}, type: {} }}"
    , obj.id, obj.width, obj.height, obj.GetTypeName());
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