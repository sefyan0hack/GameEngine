#pragma once

#include "ShaderProgram.hpp"
#include "Camera.hpp"
#include "Material.hpp"

#include <format>
#include <memory>
#include <string>

#include <engine_export.h>

class ENGINE_EXPORT SkyBox
{
public:
  SkyBox();
  SkyBox(std::shared_ptr<class TextureCubeMap> texture);

  auto set_texture(std::shared_ptr<class TextureCubeMap> texture) -> void;
  auto render(const Camera& cam) -> void;
private:
  ShaderProgram m_Program;
  std::shared_ptr<class TextureCubeMap> m_Texture;
  GLuint VAO;
};