#include "Texture.hpp"

#include "gl.hpp"

#include <core/Log.hpp>
#include <core/Exception.hpp>
#include <core/res.hpp>

#include <memory>
#include <optional>
#include <vector>
#include <array>

namespace {
constexpr auto to_string(Texture::Type type) -> const char*
{
  switch(type){
    case Texture::Type::Texture2D: return "Texture2D";
    case Texture::Type::Texture3D: return "Texture3D";
    case Texture::Type::Texture2DArray: return "Texture2DArray";
    case Texture::Type::TextureCubeMap: return "TextureCubeMap";
    default: return "UNKNOWN";
  }
}

constexpr auto to_gl(Texture::Type type) -> uint32_t
{
  switch(type){
    case Texture::Type::Texture2D: return GL_TEXTURE_2D;
    case Texture::Type::Texture3D: return GL_TEXTURE_3D;
    case Texture::Type::Texture2DArray: return GL_TEXTURE_2D_ARRAY;
    case Texture::Type::TextureCubeMap: return GL_TEXTURE_CUBE_MAP;
    default: throw 1;
  }

}

}

Texture::Texture(const char* name, Type type)
    : m_Id(0)
    , m_Type(type)
{
    auto gl_type = to_gl(m_Type);

    gl::GenTextures(1, &m_Id);
    gl::BindTexture(gl_type, m_Id);

    switch(m_Type){
        case Texture::Type::Texture2D: make_texture_Texture2D(name); break;
        case Texture::Type::Texture3D: make_texture_Texture3D(name); break;
        case Texture::Type::Texture2DArray: make_texture_Texture2DArray(name); break;
        case Texture::Type::TextureCubeMap: make_texture_TextureCubeMap(name); break;
        default: throw "texture type not supported";
    }

    gl::label_texture(m_Id, name);
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
    gl::BindTexture(to_gl(m_Type), m_Id);
}

auto Texture::type() const -> Type
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


auto Texture::texture_2d(const char* name) -> std::shared_ptr<Texture>
{
    return std::make_shared<Texture>(name, Texture::Type::Texture2D);
}

auto Texture::texture_cubemap(const char* name) -> std::shared_ptr<Texture>
{
    return std::make_shared<Texture>(name, Texture::Type::TextureCubeMap);
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

auto Texture::make_texture_Texture2D(const char* name) -> void
{
    std::optional<Image> img;

    if (name) img = Image(name);
    else img = Image();

    storage2d(std::bit_cast<const uint8_t*>(img->data().data()), to_gl(m_Type), img->width(), img->height(), gl_internal_format(img->format()), gl_format(img->format()));
    gl::GenerateMipmap(to_gl(m_Type));
}

auto Texture::make_texture_TextureCubeMap(const char* name) -> void
{
    std::vector<std::string> faces;

    if(name) {
        std::string path = name;

        std::array<std::string, 6> directions = {"posx", "negx", "posy", "negy", "posz", "negz"};
        auto dot = path.find_last_of(".");

        auto ext = path.substr(dot, path.size());

        for (const auto &dir : directions) {
            auto newPath = path.substr(0, dot);
            newPath += "_" + dir + ext;
            faces.push_back(newPath);
        }
    }

    std::optional<std::array<Image, 6>> imgs;

    if(name && faces.size() == 6) {
        imgs = std::array<Image, 6>{
            Image(faces[0].c_str(), false), Image(faces[1].c_str(), false), Image(faces[2].c_str(), false),
            Image(faces[3].c_str(), false), Image(faces[4].c_str(), false), Image(faces[5].c_str(), false)
        };
    } else {
        imgs = std::array<Image, 6>{
            Image(), Image(), Image(),
            Image(), Image(), Image()
        };
    }

    for (std::size_t i = 0; i < imgs->size(); ++i) {
        auto& img = imgs.value()[i];
        storage2d(img.data().data(), uint32_t(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i), img.width(), img.height(), gl_internal_format(img.format()), gl_format(img.format()));
    }

    gl::TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    gl::TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    gl::TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    gl::TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    gl::TexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    gl::GenerateMipmap(to_gl(m_Type));
}

auto Texture::make_texture_Texture3D(const char* name) -> void
{
    throw "unimplemended";
}

auto Texture::make_texture_Texture2DArray(const char* name) -> void
{
    throw "unimplemended";
}
