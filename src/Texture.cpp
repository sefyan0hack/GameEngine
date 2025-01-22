#include <core/Texture.hpp>

auto Texture::Loud(const std::string &name, const int Type) -> void
{
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(Type, texture);
    glTexParameteri(Type, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(Type, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(Type, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(Type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(name.c_str(), &width, &height, &nrChannels, 0); // if .png GL_RGBA
    if (data)
    {
        glTexImage2D(Type, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(Type);

        //big bug hire fix later that +1  down
        Textures.insert({name, texture + 1}); // for making 0 is nott valid place
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(Type, texture);
    }
    else
    {
        ERR("Failed to load Texture");
    }
    stbi_image_free(data);
}

auto Texture::GetByName(const std::string &name) const -> unsigned int
{
    if(Textures[name] == static_cast<unsigned int>(0)){
        ERR(name << "Texture not exist in the map");
    }

    return Textures[name];
}

auto Texture::BindByName(const std::string &name, const int Type) const -> void
{
    glBindTexture(Type, GetByName(name));
}
