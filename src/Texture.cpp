#include <core/Texture.hpp>
#include <core/Log.hpp>
#include <core/Utils.hpp>
#include <optional>
#include <tuple>
#include <stb_image.h>

namespace {
auto load_img(const char* name) -> std::optional<std::tuple<GLsizei, GLsizei, GLsizei, GLubyte*>>
{
    GLsizei width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    GLubyte *data = stbi_load(name, &width, &height, &nrChannels, 0);

    if(data != nullptr) return std::make_tuple(width, height, nrChannels, data);
    else return  std::nullopt;
}

}
Texture::Texture(const std::string &name, const GLenum Type)
    : id(0)
    , type(Type)
    , width(0)
    , height(0)
    , data(nullptr)
    , mipmapped(true)
{
    glGenTextures(1, &id);
    Bind();

    glTexParameteri(type, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(type, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    auto op = load_img(name.c_str());
    if (op)
    {
        const auto [Width, Height, Channel, Data] = op.value();
        width = Width;
        height = Height;
        data = Data;

        
        if(is_odd(width)){
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        }

        if (Channel == 4){
            glTexImage2D(type, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data); // png
        }
        else if (Channel == 3){
            glTexImage2D(type, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data); // jpeg
        }
        else {
            Log::Error("Unsupported img format num of channels are {}", Channel);
        }

        if(mipmapped){
            GenerateMipMap();
        }
        
        glActiveTexture(GL_TEXTURE1);
            
    }
    else
    {
        Log::Error("Failed to load Texture");
    }
    
    Log::Info("{}", *this);
}

auto Texture::Getid() const -> GLuint
{
    return id;
}

auto Texture::Bind() const -> void
{
    glBindTexture(type, id);
}

Texture::~Texture()
{
    if (data)
        stbi_image_free(data);
}
auto Texture::GenerateMipMap() -> void
{
    glGenerateMipmap(type);
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

auto Texture::isMipMapped() const -> GLboolean
{
    return mipmapped;
}

