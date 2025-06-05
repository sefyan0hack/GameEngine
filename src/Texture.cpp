#include <core/Texture.hpp>
#include <core/Log.hpp>
#include <core/Utils.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace {

constexpr auto to_string(GLenum type) -> const char*
{
  switch(type){
    case GL_TEXTURE_1D: return "GL_TEXTURE_1D";
    case GL_TEXTURE_2D: return "GL_TEXTURE_2D";
    case GL_TEXTURE_3D: return "GL_TEXTURE_3D";
    case GL_TEXTURE_1D_ARRAY: return "GL_TEXTURE_1D_ARRAY";
    case GL_TEXTURE_2D_ARRAY: return "GL_TEXTURE_2D_ARRAY";
    case GL_TEXTURE_RECTANGLE: return "GL_TEXTURE_RECTANGLE";
    case GL_TEXTURE_CUBE_MAP: return "GL_TEXTURE_CUBE_MAP";
    case GL_TEXTURE_CUBE_MAP_ARRAY: return "GL_TEXTURE_CUBE_MAP_ARRAY";
    case GL_TEXTURE_BUFFER: return "GL_TEXTURE_BUFFER";
    case GL_TEXTURE_2D_MULTISAMPLE: return "GL_TEXTURE_2D_MULTISAMPLE";
    case GL_TEXTURE_2D_MULTISAMPLE_ARRAY: return "GL_TEXTURE_2D_MULTISAMPLE_ARRAY";
    default: return "UNKNOWN";
  }
}

[[nodiscard]] auto load_img(const char* name, bool flip = true) -> std::optional<Image>
{
    Image img{};

    stbi_set_flip_vertically_on_load(flip);

    img.data = stbi_load(name, &img.width, &img.height, &img.Channels, 0);

    if(img.data == nullptr) return std::nullopt;
    
    return img;
}

// check error for this later
auto ubyte_to_vector(GLubyte* data, GLsizei size) -> std::vector<GLubyte>
{
  std::vector<GLubyte> result(data, data + size); 
  stbi_image_free(data);
  
  return result;
}

}
Texture::Texture(GLenum texType)
    : m_Id(0)
    , m_Type(texType)
    , m_Width(1)
    , m_Height(1)
    , m_TextureUnit(m_TextureUnitCount++)
{
    gl::GenTextures(1, &m_Id);
    Bind();
}

auto Texture::id() const -> GLuint
{
    return m_Id;
}

auto Texture::Bind() const -> void
{
    gl::ActiveTexture(static_cast<GLenum>(GL_TEXTURE0 + m_TextureUnit));
    gl::BindTexture(m_Type, m_Id);
}

auto Texture::UnBind() const -> void
{
    gl::BindTexture(m_Type, 0);
}


auto Texture::Width() const -> GLsizei
{
    return m_Width;
}

auto Texture::Height() const -> GLsizei
{
    return m_Height;
}

auto Texture::Type() const -> GLenum
{
    return m_Type;
}
auto Texture::TypeName() const -> std::string
{
    return to_string(m_Type);
}
auto Texture::TextureUnit() const -> GLint
{
    return m_TextureUnit;
}

//////////
Texture2D::Texture2D(const std::string &name)
    : Texture(GL_TEXTURE_2D), m_Mipmapped(true)
{

    gl::TexParameteri(m_Type, GL_TEXTURE_WRAP_S, GL_REPEAT);
    gl::TexParameteri(m_Type, GL_TEXTURE_WRAP_T, GL_REPEAT);
    gl::TexParameteri(m_Type, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    gl::TexParameteri(m_Type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    
    if (auto result = load_img(name.c_str()); result)
    {
        auto [Width, Height, Channel, Data] = result.value();
        m_Width = Width;
        m_Height = Height;
        m_Data = ubyte_to_vector(Data, Width * Height * Channel);
        GLenum format = Channel == 1 ? GL_RED : Channel == 3 ? GL_RGB : GL_RGBA;
        if(is_odd(m_Width)){
            gl::PixelStorei(GL_UNPACK_ALIGNMENT, 1);
        }
        gl::TexImage2D(m_Type, 0, static_cast<GLint>(format), m_Width, m_Height, 0, format, GL_UNSIGNED_BYTE, m_Data.data());

        if (m_Mipmapped) GenerateMipMap();

        Info("Loding {} ", name);
    }
    else
    {
        gl::TexStorage2D(m_Type, 1, GL_RGB32F, m_Width, m_Height);
        GLfloat clearColor[3] = { 1.0f, 0.0f, 1.0f};
        gl::ClearTexImage(m_Id, 0, GL_RGB, GL_FLOAT, clearColor); 

        Info("{}", std::format("failed to load {} : {}", name, stbi_failure_reason()));
    }

    Info("{}", static_cast<const Texture&>(*this));
}

auto Texture2D::GenerateMipMap() -> void
{
    gl::GenerateMipmap(m_Type);
}

auto Texture2D::isMipMapped() const -> GLboolean
{
    return m_Mipmapped;
}

//////
TextureCubeMap::TextureCubeMap(const std::vector<std::string> faces)
    : Texture(GL_TEXTURE_CUBE_MAP)
{

    gl::TexParameteri(m_Type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    gl::TexParameteri(m_Type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    gl::TexParameteri(m_Type, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    gl::TexParameteri(m_Type, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    gl::TexParameteri(m_Type, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    for (size_t i = 0; i < faces.size(); ++i) {
        const auto& face = faces[i];

        if (auto result = load_img(face.c_str(), false); result) {
            auto [Width, Height, Channel, Data] = result.value();
            
            std::vector<GLubyte> DataFace = ubyte_to_vector(Data, Width * Height * Channel);

            m_Width = Width;
            m_Height = Height;
            m_Data[i] = DataFace;
    
            GLenum format = Channel == 1 ? GL_RED : Channel == 3 ? GL_RGB : GL_RGBA;
            
            GLint rowBytes = Width * Channel;
            GLint alignment = (rowBytes % 8 == 0)? 8 : (rowBytes % 4 == 0)? 4 : (rowBytes % 2 == 0)? 2 : 1;
            gl::PixelStorei(GL_UNPACK_ALIGNMENT, alignment);

            gl::TexImage2D(static_cast<GLenum>(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i), 0, static_cast<GLint>(format), m_Width, m_Height, 0, format, GL_UNSIGNED_BYTE, DataFace.data());

            Info("Loding {} ", face);
        }else{
            Error("{}", std::format("failed to load {} : {}", face, stbi_failure_reason()));
        }
    }
    gl::PixelStorei(GL_UNPACK_ALIGNMENT, 4);
    Info("{}", static_cast<const Texture&>(*this));
}