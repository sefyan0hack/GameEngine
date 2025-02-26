#include <core/Texture.hpp>
#include <core/Log.hpp>
#include <core/Utils.hpp>
#include <expected>
#include <stb_image.h>
#include <ranges>

namespace {

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
    : id(0)
    , type(texType)
    , width(1)
    , height(1)
{
    glGenTextures(1, &id);
    Bind();
}

auto Texture::Getid() const -> GLuint
{
    return id;
}

auto Texture::Bind() const -> void
{
    glBindTexture(type, id);
}

auto Texture::UnBind() const -> void
{
    glBindTexture(type, 0);
}

Texture::~Texture()
{
    if(glIsTexture(id) == GL_TRUE) glDeleteTextures(1, &id);
}

auto Texture::GetWidth() const -> GLsizei
{
    return width;
}

auto Texture::GetHeight() const -> GLsizei
{
    return height;
}

auto Texture::GetType() const -> GLenum
{
    return type;
}
auto Texture::GetTypeName() const -> std::string
{
    return TEXTURETYPE[type];
}

//////////
Texture2D::Texture2D(const std::string &name)
    : Texture(GL_TEXTURE_2D), mipmapped(true)
{

    glTexParameteri(type, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(type, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    
    if (auto result = load_img(name.c_str()); result)
    {
        auto [Width, Height, Channel, Data] = result.value();
        width = Width;
        height = Height;
        data = std::move(ubyte_to_vector(Data, Width * Height * Channel));
        auto format = Channel == 1 ? GL_RED : Channel == 3 ? GL_RGB : GL_RGBA;
        if(is_odd(width)){
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        }
        glTexImage2D(type, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data.data());

        if (mipmapped) GenerateMipMap();

        glActiveTexture(GL_TEXTURE1);
        Log::Info("Loding {} ", name);
    }
    else
    {
        glTexStorage2D(type, 1, GL_RGB32F, width, height);
        GLfloat clearColor[3] = { 1.0f, 0.0f, 1.0f};
        glClearTexImage(id, 0, GL_RGB, GL_FLOAT, clearColor); 
        
        Log::Warning("{}", result.error());
    }

    Log::Info("{}", static_cast<const Texture&>(*this));
}

auto Texture2D::GenerateMipMap() -> void
{
    glGenerateMipmap(type);
}

auto Texture2D::isMipMapped() const -> GLboolean
{
    return mipmapped;
}

//////
TextureCubeMap::TextureCubeMap(const std::vector<std::string> faces)
    : Texture(GL_TEXTURE_CUBE_MAP)
{

    glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(type, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(type, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(type, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    for (auto [face, dataface, i] : std::views::zip(faces, data, std::views::iota(0)))
    {

        if (auto result = load_img(face.c_str(), false); result)
        {
            auto [Width, Height, Channel, Data] = result.value();

            width = Width;
            height = Height;
            dataface = std::move(ubyte_to_vector(Data, Width * Height * Channel));

            auto format = Channel == 1 ? GL_RED : Channel == 3 ? GL_RGB : GL_RGBA;
            
            GLint rowBytes = Width * Channel;
            GLuint alignment = (rowBytes % 8 == 0)? 8 : (rowBytes % 4 == 0)? 4 : (rowBytes % 2 == 0)? 2 : 1;
            glPixelStorei(GL_UNPACK_ALIGNMENT, alignment);

            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, dataface.data());

            Log::Info("Loding {} ", face);
        }else{
            Log::Error("{}", result.error());
        }
    }
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    Log::Info("{}", static_cast<const Texture&>(*this));
}