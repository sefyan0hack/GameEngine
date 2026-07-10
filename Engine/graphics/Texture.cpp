#include "Texture.hpp"

#include "gl.hpp"

#include <core/Log.hpp>
#include <core/Exception.hpp>
#include <core/res.hpp>

namespace {
constexpr auto to_string(uint32_t type) -> const char*
{
  switch(type){
    case GL_TEXTURE_2D: return "GL_TEXTURE_2D";
    case GL_TEXTURE_3D: return "GL_TEXTURE_3D";
    case GL_TEXTURE_2D_ARRAY: return "GL_TEXTURE_2D_ARRAY";
    case GL_TEXTURE_CUBE_MAP: return "GL_TEXTURE_CUBE_MAP";
    default: return "UNKNOWN";
  }
}

}

Texture::Texture(uint32_t texType)
    : m_Id(0)
    , m_Type(texType)
{
    gl::GenTextures(1, &m_Id);
    bind();
}

Texture::Texture(Texture&& other) noexcept : m_Id(other.m_Id), m_Type(other.m_Type) {
    other.m_Id = 0;
}

Texture& Texture::operator=(Texture&& other) noexcept {
    if (this != &other) {
        gl::DeleteTextures(1, &m_Id);
        m_Id = other.m_Id;
        m_Type = other.m_Type;
        other.m_Id = 0;
    }
    return *this;
}

Texture::~Texture() {
    gl::DeleteTextures(1, &m_Id);
}

auto Texture::id() const -> uint32_t
{
    return m_Id;
}

auto Texture::bind() const -> void
{
    gl::BindTexture(m_Type, m_Id);
}

auto Texture::type() const -> uint32_t
{
    return m_Type;
}

auto Texture::gl_format(Image::Format fmt) -> uint32_t
{
    switch (fmt) {
        case Image::Format::RED: return GL_RED;
        case Image::Format::RG: return GL_RG;
        case Image::Format::RGB: return GL_RGB;
        case Image::Format::RGBA: return GL_RGBA;
        default: return 0;
    }
}

auto Texture::gl_internal_format(Image::Format fmt) -> uint32_t
{
    switch (fmt) {
        case Image::Format::RED: return GL_R8;
        case Image::Format::RG: return GL_RG8;
        case Image::Format::RGB: return GL_RGB8;
        case Image::Format::RGBA: return GL_RGBA8;
        default: return 0;
    }
}

auto Texture::type_name() const -> std::string
{
    return to_string(m_Type);
}

Texture2D::Texture2D()
    : Texture(GL_TEXTURE_2D)
{
    storage2d(std::bit_cast<const uint8_t*>(m_Img.data().data()), m_Type, m_Img.width(), m_Img.height(), gl_internal_format(m_Img.format()), gl_format(m_Img.format()));
    gl::GenerateMipmap(m_Type);
}

//////////
Texture2D::Texture2D(const char* name)
    : Texture(GL_TEXTURE_2D), m_Img(name)
{
    storage2d(std::bit_cast<const uint8_t*>(m_Img.data().data()), m_Type, m_Img.width(), m_Img.height(), gl_internal_format(m_Img.format()), gl_format(m_Img.format()));
    gl::GenerateMipmap(m_Type);

    gl::label_texture(m_Id, name);
}

Texture2D::Texture2D(auto* data, int32_t width, int32_t height, Image::Format fmt)
    : Texture(GL_TEXTURE_2D), m_Img(data, width, height, fmt)
{
    storage2d(data, width, height, gl_internal_format(m_Img.format()), gl_format(m_Img.format()));
}

auto Texture::storage2d(const auto *data, uint32_t type, int32_t width, int32_t height, int32_t intformat, uint32_t format) -> void
{
    using DataType = std::remove_cv_t<
            std::remove_all_extents_t<
                std::remove_pointer_t<decltype(data)>
            >
        >;

    if constexpr(!std::is_arithmetic_v<DataType>){
        throw Exception("only arithmetic Types");
    }

    auto gl_type = []() -> uint32_t {
        if constexpr(std::is_same_v<DataType, uint8_t>)  return GL_UNSIGNED_BYTE;
        if constexpr(std::is_same_v<DataType, int8_t>)   return GL_BYTE;
        if constexpr(std::is_same_v<DataType, uint16_t>) return GL_UNSIGNED_SHORT;
        if constexpr(std::is_same_v<DataType, int16_t>)  return GL_SHORT;
        if constexpr(std::is_same_v<DataType, uint32_t>) return GL_UNSIGNED_INT;
        if constexpr(std::is_same_v<DataType, int32_t>)  return GL_INT;
        if constexpr(std::is_same_v<DataType, float>)    return GL_FLOAT;
    }();

    if constexpr (sizeof(DataType) >= 4) {
        //  4+ byte types (int, float), using 4-byte alignment
        gl::PixelStorei(GL_UNPACK_ALIGNMENT, 4);
    } else {
        //  smaller types, calculate optimal alignment
        const int rowBytes = width * sizeof(DataType);
        int alignment = 1;
        if (rowBytes % 8 == 0) alignment = 8;
        else if (rowBytes % 4 == 0) alignment = 4;
        else if (rowBytes % 2 == 0) alignment = 2;
        gl::PixelStorei(GL_UNPACK_ALIGNMENT, alignment);
    }

    gl::TexImage2D(type, 0, intformat, width, height, 0, format, gl_type, data);
}

//////
TextureCubeMap::TextureCubeMap()
    : Texture(GL_TEXTURE_CUBE_MAP)
{
    for (std::size_t i = 0; i < m_Imgs.size(); ++i) {
        const auto& img = m_Imgs[i];
        storage2d(img.data().data(), uint32_t(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i), img.width(), img.height(), gl_internal_format(img.format()), gl_format(img.format()));
    }

    gl::TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    gl::TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    gl::TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    gl::TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    gl::TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    gl::GenerateMipmap(m_Type); 
}

TextureCubeMap::TextureCubeMap(const std::vector<std::string> faces)
    : Texture(GL_TEXTURE_CUBE_MAP)
{
    m_Imgs = std::array<Image, 6>{
            Image(faces[0].c_str(), false), Image(faces[1].c_str(), false), Image(faces[2].c_str(), false),
            Image(faces[3].c_str(), false), Image(faces[4].c_str(), false), Image(faces[5].c_str(), false)
        };

    for (std::size_t i = 0; i < m_Imgs.size(); ++i) {
        auto& img = m_Imgs[i];
        storage2d(img.data().data(), uint32_t(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i), img.width(), img.height(), gl_internal_format(img.format()), gl_format(img.format()));
        logg::trace("Loding {} ", faces[i]);
    }

    gl::TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    gl::TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    gl::TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    gl::TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    gl::TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    gl::GenerateMipmap(m_Type);

    gl::label_texture(m_Id, faces[0].c_str());
}

auto TextureCubeMap::base_to_6faces(const std::string& path) -> std::vector<std::string>
{
    std::vector<std::string> result;
    std::array<std::string, 6> directions = {"posx", "negx", "posy", "negy", "posz", "negz"};
    auto dot = path.find_last_of(".");
    auto ext = path.substr(dot, path.size());

    for (const auto &dir : directions) {
        auto newPath = path.substr(0, dot);
        newPath += "_" + dir + ext;
        result.push_back(newPath);
    }

    return result;
}