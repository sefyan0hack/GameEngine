#pragma once
#include <memory>

#include <engine_export.h>

class ENGINE_EXPORT Material
{
public:
    Material();
    Material(std::shared_ptr<class Texture> diffuse);

    auto diffuse() const noexcept -> std::shared_ptr<class Texture>;
    auto set_diffuse(std::shared_ptr<class Texture> texture) -> void;

private:
    std::shared_ptr<class Texture> m_Diffuse;
};
