#pragma once
#include <format>
#include <memory>
#include <string>
#include <engine_export.h>
#include "Shader.hpp"
#include "ShaderProgram.hpp"
#include "Mesh.hpp"

class TextureCubeMap;

class ENGINE_EXPORT SkyBox
{
  friend class Scene;

  SkyBox();
  SkyBox(std::shared_ptr<TextureCubeMap> texture);
  SkyBox(const std::string& BasePathName);

  auto set_texture(std::shared_ptr<TextureCubeMap> texture) -> void;
  auto set_texture(const std::string& BasePathName) -> void;

private:
  Shader m_VertShader;
  Shader m_FragShader;
  ShaderProgram m_Program;
  Mesh m_Mesh;
  std::shared_ptr<TextureCubeMap> m_Texture;
};