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
    virtual ~Texture() = default;
public:
    virtual auto id() const -> GLuint final;
    virtual auto Bind() const -> void  final;
    virtual auto UnBind() const -> void  final;
    virtual auto Width() const -> GLsizei final;
    virtual auto Height() const -> GLsizei final;
    virtual auto Type() const -> GLenum final;
    virtual auto TypeName() const -> std::string final;
    virtual auto TextureUnit() const -> GLint final;
protected:
    GLuint m_Id;
    GLenum m_Type;
    GLsizei m_Width, m_Height;
    GLint m_TextureUnit;
    inline static GLint m_TextureUnitCount = 0;
};

class Texture2D final : public Texture
{
  public:
    Texture2D(const std::string &name);
    auto isMipMapped() const -> GLboolean;
  private:
    auto GenerateMipMap() -> void ;
  
    std::vector<GLubyte> m_Data;
    GLboolean m_Mipmapped;
};

class TextureCubeMap final : public Texture
{
  public:
    TextureCubeMap(const std::vector<std::string> faces);
  private:
    std::array<std::vector<GLubyte>, 6> m_Data;
};

// custom Texture Format
template<>
struct std::formatter<Texture> {
  constexpr auto parse(std::format_parse_context& context) {
    return context.begin();
  }
  auto format(const Texture& obj, std::format_context& context) const {
    return std::format_to(context.out(),
    R"({{ "id": {}, "width": {}, "height": {}, "type": "{}" }})"
    , obj.m_Id, obj.m_Width, obj.m_Height, obj.TypeName());
  }
};