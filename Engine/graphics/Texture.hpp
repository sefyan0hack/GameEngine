#pragma once
#include <string>
#include <format>
#include <vector>
#include <array>
#include <engine_export.h>

#include "Image.hpp"
#include "gl.hpp"

#include <cmrc/cmrc.hpp>

class ENGINE_EXPORT Texture
{
public:
    friend struct std::formatter<Texture>;
protected:
    Texture(GLenum texType);
    Texture(const Texture&) = delete;
    auto operator=(const Texture&) -> Texture& = delete;
    
    Texture(Texture&&) noexcept;
    auto operator=(Texture&&) noexcept-> Texture&;
    ~Texture();

public:
    auto id() const -> GLuint;
    auto bind() const -> void;
    auto type() const -> GLenum;
    auto type_name() const -> std::string;

    auto img2d_to_gpu(const auto* data, GLsizei width, GLsizei height, GLint intformat = GL_RGBA8, GLenum format = GL_RGBA) const -> void;

protected:
    GLuint m_Id;
    GLenum m_Type;
};

class ENGINE_EXPORT Texture2D final : public Texture
{
  public:
    Texture2D();
    Texture2D(const Texture2D&) = delete;
    auto operator=(const Texture2D&) -> Texture2D& = delete;
    Texture2D(const std::string &name);
    Texture2D(const cmrc::file &src);
    Texture2D(auto* data, GLint width, GLint height, GLenum format = GL_RGBA);

  private:
    Image m_Img;
};

class ENGINE_EXPORT TextureCubeMap final : public Texture
{
  public:
    TextureCubeMap();
    TextureCubeMap(const TextureCubeMap&) = delete;
    auto operator=(const TextureCubeMap&) -> TextureCubeMap& = delete;
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
    R"({{ "id": {}, "type": "{}" }})"
    , obj.m_Id, obj.type_name());
  }
};