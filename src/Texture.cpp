#include <core/Texture.hpp>
#include <core/Log.hpp>
#include <core/Utils.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace {
    template <typename T, std::size_t Width, std::size_t Height>
    constexpr auto generate_checkerboard(const uint32_t color1, const uint32_t color2) {
        constexpr std::size_t color_width = sizeof(color1);

        std::array<T, Width * Height * color_width> data{};
        
        for (std::size_t y = 0; y < Height; ++y) {
            for (std::size_t x = 0; x < Width; ++x) {
                const bool is_color1 = ( (x/5 + y/5) % 2) == 0;
                const uint32_t color = is_color1 ? color1 : color2;
                
                const std::size_t base_index = (y * Width + x) * color_width;
                
                for(std::size_t i = 0; i < color_width; i++){
                    data[base_index + i] = static_cast<T>((color >> (8 * i)));
                }
            }
        }
        return data;
    }

    // magenta/gray checkerboard 20x20
    constexpr auto checkerboard = generate_checkerboard<uint32_t, 20, 20>(0xFFFF00FF, 0xFF808080);

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
template <typename T, std::size_t N>
auto ubyte_to_vector(const std::array<T, N>& data) -> std::vector<GLubyte>
{
  std::vector<GLubyte> result(data.data(), data.data() + (N * sizeof(T)));
  return result;
}

}

Texture2D Texture2D::Default{
    checkerboard.data(),
    20, 20
};

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
    // static bool once = false;
    // if(!once){
    //     gl::TexParameteri(m_Type, GL_TEXTURE_WRAP_S, GL_REPEAT);
    //     gl::TexParameteri(m_Type, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //     gl::TexParameteri(m_Type, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    //     gl::TexParameteri(m_Type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //     once = true;
    // }
    GLint internalFormat{};
    GLenum format = GL_NONE;

    if (auto result = load_img(name.c_str()); result)
    {
        auto [Width, Height, Channel, Data] = result.value();
        m_Width = Width;
        m_Height = Height;
        m_Data = ubyte_to_vector(Data, m_Width * m_Height * Channel);

        internalFormat  = 
            Channel == 1 ? GL_RED :
            Channel == 3 ? GL_RGB8 : GL_RGBA8;

        format  = 
            Channel == 1 ? GL_RED :
            Channel == 3 ? GL_RGB : GL_RGBA;

        Info("Loding {} ", name);
    }
    else
    {
        m_Width = 20;
        m_Height = 20;
        m_Data = ubyte_to_vector(checkerboard);
        internalFormat = GL_RGBA8;
        format = GL_RGBA;
        Info("{}", std::format("failed to load {} : {}", name, stbi_failure_reason()));
    }
    
    ToGPUImg2D( m_Data.data(), m_Width, m_Height, internalFormat, format);

    if (m_Mipmapped) GenerateMipMap();

    Info("{}", static_cast<const Texture&>(*this));
}

Texture2D::Texture2D(auto* data, GLint width, GLint height, GLenum intformat, GLenum format)
    : Texture(GL_TEXTURE_2D), m_Mipmapped(true)
{
    ToGPUImg2D(data, width, height, intformat, format);
}

auto Texture::ToGPUImg2D(auto *data, GLsizei width, GLsizei height, GLint intformat, GLenum format) const -> void
{
    using DataType = std::remove_cv_t<
            std::remove_all_extents_t<
                std::remove_pointer_t<decltype(data)>
            >
        >;

    if constexpr(!std::is_arithmetic_v<DataType>){
        Error("`{}` not allowd only primitives Types", ::type_name<DataType>());
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
        else Error("unreachable");;
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
    // static bool once = false;
    // if(!once){
    //     gl::TexParameteri(m_Type, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    //     gl::TexParameteri(m_Type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //     gl::TexParameteri(m_Type, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    //     gl::TexParameteri(m_Type, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //     gl::TexParameteri(m_Type, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    //     once = true;
    // }
    

    for (std::size_t i = 0; i < faces.size(); ++i) {
        const auto& face = faces[i];

        if (auto result = load_img(face.c_str(), false); result) {
            auto [Width, Height, Channel, Data] = result.value();
            
            std::vector<GLubyte> DataFace = ubyte_to_vector(Data, Width * Height * Channel);

            m_Width = Width;
            m_Height = Height;
            m_Data[i] = DataFace;
    
            GLenum internalFormat  = 
                    Channel == 1 ? GL_RED :
                    Channel == 3 ? GL_RGB8 : GL_RGBA8;

            GLenum format  = 
                    Channel == 1 ? GL_RED :
                    Channel == 3 ? GL_RGB : GL_RGBA;
            
            GLint rowBytes = Width * Channel;
            GLint alignment = (rowBytes % 8 == 0)? 8 : (rowBytes % 4 == 0)? 4 : (rowBytes % 2 == 0)? 2 : 1;
            gl::PixelStorei(GL_UNPACK_ALIGNMENT, alignment);

            gl::TexImage2D(static_cast<GLenum>(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i), 0, static_cast<GLint>(internalFormat), m_Width, m_Height, 0, format, GL_UNSIGNED_BYTE, DataFace.data());

            Info("Loding {} ", face);
        }else{
            Error("{}", std::format("failed to load {} : {}", face, stbi_failure_reason()));
        }
    }
    gl::GenerateMipmap(m_Type);
    gl::PixelStorei(GL_UNPACK_ALIGNMENT, 4);
    Info("{}", static_cast<const Texture&>(*this));
}