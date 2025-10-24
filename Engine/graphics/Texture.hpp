#pragma once
#include <string>
#include <format>
#include <vector>
#include <array>

#include "Image.hpp"
#include "gl.h"

#include <cmrc/cmrc.hpp>

class Texture
{
public:
    friend struct std::formatter<Texture>;
    Texture(GLenum texType);
    virtual ~Texture() = default;
public:
    virtual auto id() const -> GLuint final;
    virtual auto bind() const -> void  final;
    virtual auto type() const -> GLenum final;
    virtual auto type_name() const -> std::string final;
    virtual auto texture_unit() const -> GLint final;

    auto img2d_to_gpu(auto* data, GLsizei width, GLsizei height, GLint intformat = GL_RGBA8, GLenum format = GL_RGBA) const -> void;
protected:
    GLuint m_Id;
    GLenum m_Type;
    GLint m_TextureUnit;
    inline static GLint m_TextureUnitCount = 0;
    
};

class Texture2D final : public Texture
{
  public:
    Texture2D();
    Texture2D(const std::string &name);
    Texture2D(const cmrc::file &src);
    Texture2D(auto* data, GLint width, GLint height, GLenum format = GL_RGBA);
    auto is_mipmapped() const -> bool;

  private:
    auto generate_mipmap() -> void ;

    Image m_Img;
    bool m_Mipmapped;
};

class TextureCubeMap final : public Texture
{
  public:
    TextureCubeMap();
    TextureCubeMap(const std::vector<std::string> faces);
    TextureCubeMap(const std::vector<cmrc::file>& faces);

    static auto base_to_6faces(const std::string& path) -> std::vector<std::string>;
    static auto base_to_6facesfiles(const std::string& path) -> std::vector<cmrc::file>;

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
    , obj.m_Id, obj.type_name(), obj.m_TextureUnit, Texture::m_TextureUnitCount);
  }
};