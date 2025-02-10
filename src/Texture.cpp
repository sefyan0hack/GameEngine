#include <core/Texture.hpp>
#include <core/Log.hpp>
#include <core/Utils.hpp>
#include <optional>
#include <tuple>
#include <stb_image.h>

namespace {
auto load_img(const char* name, bool flip = true) -> std::optional<std::tuple<GLsizei, GLsizei, GLsizei, std::vector<GLubyte>>>
{
    stbi_set_flip_vertically_on_load(flip);
    GLsizei width = 0, height = 0, nrChannels = 0;
    GLubyte *data = stbi_load(name, &width, &height, &nrChannels, 0);

    if(data == nullptr) return std::nullopt;

    const auto size = static_cast<size_t>(width) * height * nrChannels;

    std::vector<GLubyte> result(data, data + size);;
    
    stbi_image_free(data);
    
    return std::make_tuple(width, height, nrChannels, std::move(result));
}

}
Texture::Texture(GLenum texType)
    : id(0)
    , type(texType)
    , width(0)
    , height(0)
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

    stbi_set_flip_vertically_on_load(true);
    auto op = load_img(name.c_str());
    if (op)
    {
        auto [Width, Height, Channel, Data] = op.value();
        width = Width;
        height = Height;
        data = std::move(Data);
        
        if(is_odd(width)){
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        }

        if (Channel == 4){
            glTexImage2D(type, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data()); // png
        }
        else if (Channel == 3){
            glTexImage2D(type, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data.data()); // jpeg
        }
        else {
            Log::Error("Unsupported img format num of channels are {}", Channel);
        }

        if(mipmapped){
            GenerateMipMap();
        }
        Log::Info("Loding {} ", name);
        glActiveTexture(GL_TEXTURE1);
    }
    else
    {
        Log::Error("Failed to load Texture");
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

    for (GLenum i = 0; i < faces.size(); i++)
    {
        auto face = faces[i];
        auto& dataface = data[i];

        auto op = load_img(face.c_str(), false);
        if (op)
        {
            auto [Width, Height, Channel, Data] = op.value();
            width = Width;
            height = Height;
            dataface = std::move(Data);

            if(is_odd(width)){
                glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            }

            if (Channel == 4){
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, dataface.data());// png
            }
            else if (Channel == 3){
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, dataface.data());// jpeg
            }
            else {
                Log::Error("Unsupported img format num of channels are {}", Channel);
            }
            Log::Info("Loding {} ", face);

        }else{
            Log::Error("the op is null");
        }
    }
    Log::Info("{}", static_cast<const Texture&>(*this));
}