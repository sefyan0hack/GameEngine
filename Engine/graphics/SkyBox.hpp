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
  SkyBox(std::shared_ptr<class Texture> texture);

  auto set_texture(std::shared_ptr<class Texture> texture) -> void;

private:
  ShaderProgram m_Program;
  std::shared_ptr<class Texture> m_Texture;
  GLuint m_DummyVAO;
};