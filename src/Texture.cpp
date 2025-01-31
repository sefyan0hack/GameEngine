#include <core/Texture.hpp>
#include <core/Log.hpp>
#include <stb_image.h>

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

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(name.c_str(), &width, &height, &nrChannels, 0); // if .png GL_RGBA
    if (data)
    {
        glTexImage2D(type, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(type);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(type, id);
    }
    else
    {
        Log::Error("Failed to load Texture");
    }
    stbi_image_free(data);
}

auto Texture::Getid() const -> GLuint
{
    return id;
}

auto Texture::Bind() const -> void
{
    glBindTexture(type, id);
}
