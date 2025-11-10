#pragma once
#include <format>
#include <memory>
#include <string>
#include <engine_export.h>

class ENGINE_API SkyBox
{
public:
  SkyBox();
  SkyBox(std::shared_ptr<class TextureCubeMap> texture);
  SkyBox(const std::string& BasePathName);

  SkyBox(const SkyBox&) = delete;
  auto operator=(const SkyBox&) -> SkyBox& = delete;

  auto texture() const -> std::shared_ptr<class TextureCubeMap>;
  auto mesh() const -> std::shared_ptr<class Mesh>;
  auto program() const -> std::shared_ptr<class ShaderProgram>;

private:
  const std::shared_ptr<class Shader> m_VertShader;
  const std::shared_ptr<class Shader> m_FragShader;
  const std::shared_ptr<class ShaderProgram> m_Program;
  const std::shared_ptr<class Mesh> m_Mesh;
  std::shared_ptr<class TextureCubeMap> m_Texture;
};