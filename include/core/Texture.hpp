#pragma once
#include <unordered_map>
#include <string>
#include <format>
#include <vector>
#include <array>
#include <core/gl.h>

struct Image {
  GLsizei width, height, Channels;
  GLubyte* data;
};

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
    "{}: {{ id: {}, width: {}, height: {}, type: {} }}"
    , typeid(obj).name(), obj.id, obj.width, obj.height, obj.GetTypeName());
  }
};