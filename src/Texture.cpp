#include <core/Texture.hpp>
#include <core/Log.hpp>
#include <core/Utils.hpp>
#include <expected>
#include <stb_image.h>
#include <ranges>

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

[[nodiscard]] auto load_img(const char* name, bool flip = true) -> std::expected<Image, std::string>
{
    Image img{};

    stbi_set_flip_vertically_on_load(flip);

    img.data = stbi_load(name, &img.width, &img.height, &img.Channels, 0);

    if(img.data == nullptr) return std::unexpected(std::format("failed to load {} : {}", name, stbi_failure_reason()));
    
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
    glGenTextures(1, &m_Id);
    Bind();
}

auto Texture::id() const -> GLuint
{
    return m_Id;
}

auto Texture::Bind() const -> void
{
    glActiveTexture(GL_TEXTURE0 + m_TextureUnit);
    glBindTexture(m_Type, m_Id);
}

auto Texture::UnBind() const -> void
{
    glBindTexture(m_Type, 0);
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

    glTexParameteri(m_Type, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(m_Type, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(m_Type, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(m_Type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    
    if (auto result = load_img(name.c_str()); result)
    {
        auto [Width, Height, Channel, Data] = result.value();
        m_Width = Width;
        m_Height = Height;
        m_Data = std::move(ubyte_to_vector(Data, Width * Height * Channel));
        auto format = Channel == 1 ? GL_RED : Channel == 3 ? GL_RGB : GL_RGBA;
        if(is_odd(m_Width)){
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        }
        glTexImage2D(m_Type, 0, format, m_Width, m_Height, 0, format, GL_UNSIGNED_BYTE, m_Data.data());

        if (m_Mipmapped) GenerateMipMap();

        Log::Info("Loding {} ", name);
    }
    else
    {
        glTexStorage2D(m_Type, 1, GL_RGB32F, m_Width, m_Height);
        GLfloat clearColor[3] = { 1.0f, 0.0f, 1.0f};
        glClearTexImage(m_Id, 0, GL_RGB, GL_FLOAT, clearColor); 
        
        Log::Info("{}", result.error());
    }

    Log::Info("{}", static_cast<const Texture&>(*this));
}

auto Texture2D::GenerateMipMap() -> void
{
    glGenerateMipmap(m_Type);
}

auto Texture2D::isMipMapped() const -> GLboolean
{
    return m_Mipmapped;
}

//////
TextureCubeMap::TextureCubeMap(const std::vector<std::string> faces)
    : Texture(GL_TEXTURE_CUBE_MAP)
{

    glTexParameteri(m_Type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(m_Type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(m_Type, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(m_Type, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(m_Type, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    for (auto [face, dataface, i] : std::views::zip(faces, m_Data, std::views::iota(0)))
    {

        if (auto result = load_img(face.c_str(), false); result)
        {
            auto [Width, Height, Channel, Data] = result.value();

            m_Width = Width;
            m_Height = Height;
            dataface = std::move(ubyte_to_vector(Data, Width * Height * Channel));

            auto format = Channel == 1 ? GL_RED : Channel == 3 ? GL_RGB : GL_RGBA;
            
            GLint rowBytes = Width * Channel;
            GLuint alignment = (rowBytes % 8 == 0)? 8 : (rowBytes % 4 == 0)? 4 : (rowBytes % 2 == 0)? 2 : 1;
            glPixelStorei(GL_UNPACK_ALIGNMENT, alignment);

            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, m_Width, m_Height, 0, format, GL_UNSIGNED_BYTE, dataface.data());

            Log::Info("Loding {} ", face);
        }else{
            Log::Error("{}", result.error());
        }
    }
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    Log::Info("{}", static_cast<const Texture&>(*this));
}