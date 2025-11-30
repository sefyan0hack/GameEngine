#include <core/Log.hpp>
#include <core/Utils.hpp>
#include <core/res.hpp>
#include "Texture.hpp"
#include "OpenGL.hpp"

extern cmrc::embedded_filesystem embed_filesystem;

namespace {
constexpr auto to_string(GLenum type) -> const char*
{
  switch(type){
    #if !defined(WEB_PLT)
    case GL_TEXTURE_1D: return "GL_TEXTURE_1D";
    case GL_TEXTURE_1D_ARRAY: return "GL_TEXTURE_1D_ARRAY";
    case GL_TEXTURE_BUFFER: return "GL_TEXTURE_BUFFER";
    case GL_TEXTURE_RECTANGLE: return "GL_TEXTURE_RECTANGLE";
    #endif
    case GL_TEXTURE_2D: return "GL_TEXTURE_2D";
    case GL_TEXTURE_3D: return "GL_TEXTURE_3D";
    case GL_TEXTURE_2D_ARRAY: return "GL_TEXTURE_2D_ARRAY";
    case GL_TEXTURE_CUBE_MAP: return "GL_TEXTURE_CUBE_MAP";
    case GL_TEXTURE_2D_MULTISAMPLE: return "GL_TEXTURE_2D_MULTISAMPLE";
    case GL_TEXTURE_2D_MULTISAMPLE_ARRAY: return "GL_TEXTURE_2D_MULTISAMPLE_ARRAY";
    default: return "UNKNOWN";
  }
}

}

Texture::Texture(GLenum texType)
    : m_Id(0)
    , m_Type(texType)
    , m_TextureUnit(m_TextureUnitCount++)
{
    gl::GenTextures(1, &m_Id);
    bind();
}

auto Texture::id() const -> GLuint
{
    return m_Id;
}

auto Texture::bind() const -> void
{
    gl::ActiveTexture(static_cast<GLenum>(GL_TEXTURE0 + m_TextureUnit));
    gl::BindTexture(m_Type, m_Id);
}

auto Texture::type() const -> GLenum
{
    return m_Type;
}
auto Texture::type_name() const -> std::string
{
    return to_string(m_Type);
}
auto Texture::texture_unit() const -> GLint
{
    return m_TextureUnit;
}

Texture2D::Texture2D()
    : Texture(GL_TEXTURE_2D), m_Img(), m_Mipmapped(true)
{
    img2d_to_gpu(reinterpret_cast<GLubyte*>(m_Img.data().data()), m_Img.width(), m_Img.height(), m_Img.gpu_format(), m_Img.cpu_format());
    
    if (m_Mipmapped) generate_mipmap();

    debug::print(static_cast<const Texture&>(*this));
}

//////////
Texture2D::Texture2D(const std::string &name)
    : Texture(GL_TEXTURE_2D), m_Img(name), m_Mipmapped(true)
{
    img2d_to_gpu(reinterpret_cast<GLubyte*>(m_Img.data().data()), m_Img.width(), m_Img.height(), m_Img.gpu_format(), m_Img.cpu_format());

    if (m_Mipmapped) generate_mipmap();

    debug::print(static_cast<const Texture&>(*this));
}

Texture2D::Texture2D(const cmrc::file &src)
    : Texture(GL_TEXTURE_2D), m_Img(src), m_Mipmapped(true)
{
    img2d_to_gpu(reinterpret_cast<GLubyte*>(m_Img.data().data()), m_Img.width(), m_Img.height(), m_Img.gpu_format(), m_Img.cpu_format());

    if (m_Mipmapped) generate_mipmap();

    debug::print(static_cast<const Texture&>(*this));
}

Texture2D::Texture2D(auto* data, GLint width, GLint height, GLenum format)
    : Texture(GL_TEXTURE_2D), m_Img(data, width, height, Image::channel_from_cpu_format(format)), m_Mipmapped(true)
{
    img2d_to_gpu(data, width, height, m_Img.gpu_format(), m_Img.cpu_format());
}

auto Texture::img2d_to_gpu(auto *data, GLsizei width, GLsizei height, GLint intformat, GLenum format) const -> void
{
    using DataType = std::remove_cv_t<
            std::remove_all_extents_t<
                std::remove_pointer_t<decltype(data)>
            >
        >;

    if constexpr(!std::is_arithmetic_v<DataType>){
        throw Exception("`{}` not allowd only primitives Types", ::type_name<DataType>());
    }

    auto gl_type = []() -> GLenum {
        if constexpr(std::is_same_v<DataType, GLubyte>)  return GL_UNSIGNED_BYTE;
        if constexpr(std::is_same_v<DataType, GLbyte>)   return GL_BYTE;
        if constexpr(std::is_same_v<DataType, GLushort>) return GL_UNSIGNED_SHORT;
        if constexpr(std::is_same_v<DataType, GLshort>)  return GL_SHORT;
        if constexpr(std::is_same_v<DataType, GLuint>)   return GL_UNSIGNED_INT;
        if constexpr(std::is_same_v<DataType, GLint>)    return GL_INT;
        if constexpr(std::is_same_v<DataType, GLhalf>)   return GL_HALF_FLOAT;
        if constexpr(std::is_same_v<DataType, GLfloat>)  return GL_FLOAT;
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

    gl::TexImage2D(m_Type, 0, intformat, width, height, 0, format, gl_type, data);
}

auto Texture2D::generate_mipmap() -> void
{
    gl::GenerateMipmap(m_Type);
}

auto Texture2D::is_mipmapped() const -> bool
{
    return m_Mipmapped;
}

//////
TextureCubeMap::TextureCubeMap()
    : Texture(GL_TEXTURE_CUBE_MAP), m_Imgs()
{
    for (std::size_t i = 0; i < m_Imgs.size(); ++i) {
        const auto& img = m_Imgs[i];
        gl::TexImage2D(static_cast<GLenum>(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i), 0, img.gpu_format(), img.width(), img.height(), 0, img.cpu_format(), GL_UNSIGNED_BYTE, img.data().data());
    }
}

TextureCubeMap::TextureCubeMap(const std::vector<std::string> faces)
    : TextureCubeMap()
{
    m_Imgs = std::move(
        std::array<Image, 6>{
            Image(faces[0], false), Image(faces[1], false), Image(faces[2], false),
            Image(faces[3], false), Image(faces[4], false), Image(faces[5], false)
        }
    );

    for (std::size_t i = 0; i < m_Imgs.size(); ++i) {
        auto& img = m_Imgs[i];

        GLint rowBytes = img.width() * img.channels();
        GLint alignment = (rowBytes % 8 == 0)? 8 : (rowBytes % 4 == 0)? 4 : (rowBytes % 2 == 0)? 2 : 1;
        gl::PixelStorei(GL_UNPACK_ALIGNMENT, alignment);

        gl::TexImage2D(static_cast<GLenum>(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i), 0, img.gpu_format(), img.width(), img.height(), 0, img.cpu_format(), GL_UNSIGNED_BYTE, img.data().data());

        debug::print("Loding {} ", faces[i]);
    }

    gl::GenerateMipmap(m_Type);
    gl::PixelStorei(GL_UNPACK_ALIGNMENT, 4);
    debug::print(static_cast<const Texture&>(*this));
}


TextureCubeMap::TextureCubeMap(const std::vector<cmrc::file>& faces)
    : TextureCubeMap()
{
    m_Imgs = std::move(
        std::array<Image, 6>{
            Image(faces[0], false), Image(faces[1], false), Image(faces[2], false),
            Image(faces[3], false), Image(faces[4], false), Image(faces[5], false)
        }
    );

    for (std::size_t i = 0; i < m_Imgs.size(); ++i) {
        auto& img = m_Imgs[i];

        GLint rowBytes = img.width() * img.channels();
        GLint alignment = (rowBytes % 8 == 0)? 8 : (rowBytes % 4 == 0)? 4 : (rowBytes % 2 == 0)? 2 : 1;
        gl::PixelStorei(GL_UNPACK_ALIGNMENT, alignment);

        gl::TexImage2D(static_cast<GLenum>(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i), 0, img.gpu_format(), img.width(), img.height(), 0, img.cpu_format(), GL_UNSIGNED_BYTE, img.data().data());
    }

    gl::GenerateMipmap(m_Type);
    gl::PixelStorei(GL_UNPACK_ALIGNMENT, 4);
    debug::print(static_cast<const Texture&>(*this));
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
auto TextureCubeMap::base_to_6facesfiles(const std::string& path) -> std::vector<cmrc::file>
{
    std::vector<cmrc::file> files;
    std::array<std::string, 6> directions = {"posx", "negx", "posy", "negy", "posz", "negz"};
    auto dot = path.find_last_of(".");
    auto ext = path.substr(dot, path.size());

    for (const auto &dir : directions) {
        auto newPath = path.substr(0, dot);
        newPath += "_" + dir + ext;
        files.push_back(embed_filesystem.open(newPath));
    }

    return files;
}