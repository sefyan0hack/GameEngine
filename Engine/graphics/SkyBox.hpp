#pragma once
#include <format>
#include <memory>
#include <string>
#include <engine_export.h>
#include "ShaderProgram.hpp"

class ENGINE_EXPORT SkyBox
{
  friend class Material;

  SkyBox();
  SkyBox(std::shared_ptr<class TextureCubeMap> texture);

  auto set_texture(std::shared_ptr<class TextureCubeMap> texture) -> void;

private:
  ShaderProgram m_Program;
  std::shared_ptr<class TextureCubeMap> m_Texture;
  GLuint m_DummyVAO;
};