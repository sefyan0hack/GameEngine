#pragma once
#include <string>
#include <format>
#include <vector>
#include <array>

#include <core/gl.h>
#include <core/Image.hpp>

class Texture
{
public:
    friend struct std::formatter<Texture>;
    Texture(GLenum texType);
    virtual ~Texture() = default;
public:
    virtual auto id() const -> GLuint final;
    virtual auto Bind() const -> void  final;
    virtual auto Type() const -> GLenum final;
    virtual auto TypeName() const -> std::string final;
    virtual auto TextureUnit() const -> GLint final;

    auto ToGPUImg2D(auto* data, GLsizei width, GLsizei height, GLint intformat = GL_RGBA8, GLenum format = GL_RGBA) const -> void;
protected:
    GLuint m_Id;
    GLenum m_Type;
    GLint m_TextureUnit;
    inline static GLint m_TextureUnitCount = 0;
    FOR_TEST
};

class Texture2D final : public Texture
{
  public:
    Texture2D();
    Texture2D(const std::string &name);
    Texture2D(auto* data, GLint width, GLint height, GLenum format = GL_RGBA);
    auto isMipMapped() const -> bool;

  private:
    auto GenerateMipMap() -> void ;

    Image m_Img;
    bool m_Mipmapped;
};

class TextureCubeMap final : public Texture
{
  public:
    TextureCubeMap();
    TextureCubeMap(const std::vector<std::string> faces);

    static auto base_to_6faces(const std::string& path) -> std::vector<std::string>;

  private:
    std::array<Image, 6> m_Imgs;
};

// custom Texture Format
template<>
struct std::formatter<Texture> {
  constexpr auto parse(std::format_parse_context& context) {
    return context.begin();
  }
  auto format(const Texture& obj, std::format_context& context) const {
    return std::format_to(context.out(),
    R"({{ "id": {}, "type": "{}", "uint": {}, "number": {} }})"
    , obj.m_Id, obj.TypeName(), obj.m_TextureUnit, Texture::m_TextureUnitCount);
  }
};