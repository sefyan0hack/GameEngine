#include <core/Texture.hpp>
#include <core/Log.hpp>
#include <core/Utils.hpp>
#include <optional>
#include <tuple>
#include <stb_image.h>

namespace {
auto load_img(const char* name) -> std::optional<std::tuple<GLsizei, GLsizei, GLsizei, std::vector<GLubyte>>>
{
    GLsizei width = 0, height = 0, nrChannels = 0;
    GLubyte *data = stbi_load(name, &width, &height, &nrChannels, 0);

    if(data == nullptr) return std::nullopt;

    const auto size = static_cast<size_t>(width) * height * nrChannels;

    std::vector<GLubyte> result(data, data + size);;
    
    stbi_image_free(data);
    
    return std::make_tuple(width, height, nrChannels, std::move(result));
}

}
Texture::Texture(const std::string &name, const GLenum Type)
    : id(0)
    , type(Type)
    , width(0)
    , height(0)
    , data({})
    , mipmapped(true)
{
    Log::Expect(type != GL_TEXTURE_CUBE_MAP, "type is {}", TEXTURETYPE[GL_TEXTURE_CUBE_MAP]);
    
    glGenTextures(1, &id);
    Bind();

    glTexParameteri(type, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(type, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_set_flip_vertically_on_load(true);
    auto op = load_img(name.c_str());
    if (op)
    {
        const auto [Width, Height, Channel, Data] = op.value();
        width = Width;
        height = Height;
        data = std::move(Data);

        Log::Info("lenght {}, w*h*c {}", data.size(), width*height*Channel);
        
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
        
        glActiveTexture(GL_TEXTURE1);
            
    }
    else
    {
        Log::Error("Failed to load Texture");
    }
    
    Log::Info("{}", *this);
}

Texture::Texture(const std::vector<std::string> faces)
    : id(0)
    , type(GL_TEXTURE_CUBE_MAP)
    , width(0)
    , height(0)
    , data({})
    , mipmapped(false)
{
    glGenTextures(1, &id);
    Bind();

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    stbi_set_flip_vertically_on_load(false);
    GLuint i = 0;
    for (auto face : faces)
    {
        auto op = load_img(face.c_str());
        if (op)
        {
            const auto [Width, Height, Channel, Data] = op.value();
            width = Width;
            height = Height;
            data = std::move(Data);
            
            Log::Info("lenght {}, w*h*c {}", data.size(), width*height*Channel);

            if(is_odd(width)){
                glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            }

            if (Channel == 4){
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data());// png
            }
            else if (Channel == 3){
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB8, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, data.data());// jpeg
            }
            else {
                Log::Error("Unsupported img format num of channels are {}", Channel);
            }

        }else{
            Log::Error("the op is null");
        }
        i++;
    }
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

