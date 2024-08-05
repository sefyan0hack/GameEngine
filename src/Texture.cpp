#include "Texture.hpp"

void Texture::Loud(const std::string &name, const int Type)
{
    unsigned int texture;
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0);
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
        glTexImage2D(Type, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(Type);

        Textures.insert({name, texture + 1}); // for making 0 is nott valid place
    }
    else
    {
        ERR("Failed to load Texture");
    }
    stbi_image_free(data);
}

unsigned int Texture::GetByName(const std::string &name) const
{
    if(Textures[name] == static_cast<unsigned int>(0)){
        ERR(name << "Texture not exist in the map");
    }

    return Textures[name];
}

void Texture::BindByName(const std::string &name, const int Type) const
{
    glBindTexture(Type, GetByName(name));
}
