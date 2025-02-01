#include <core/Texture.hpp>
#include <core/Log.hpp>
#include <optional>
#include <tuple>
#include <stb_image.h>

namespace {
auto load_img(const char* name) -> std::optional<std::tuple<int, int, int, GLubyte*>>
{
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    GLubyte *data = stbi_load(name, &width, &height, &nrChannels, 0);

    if(data != nullptr) return std::make_tuple(width, height, nrChannels, data);
    else return  std::nullopt;
}

}
Texture::Texture(const std::string &name, const GLenum Type)
    : id(0)
    , type(Type)
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
        const auto [width, height, channel, data] = op.value();
        Log::print("texture {} :: w: {}, h: {}, chan: {}, data: {}", id, width, height, channel, reinterpret_cast<const void*>(&data));
        if (channel == 4){
            glTexImage2D(type, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data); // png
        }
        else if (channel == 3){
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            glTexImage2D(type, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data); // jpeg
        }
        else {
            Log::Error("Unsupported img format num of channels are {}", channel);
        }
        glGenerateMipmap(type);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(type, id);
        stbi_image_free(data);
    }
    else
    {
        Log::Error("Failed to load Texture");
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
