#include "Scene.hpp"
#include "Texture.hpp"
#include "Material.hpp"
#include "GameObject.hpp"
#include "ShaderProgram.hpp"
#include "Mesh.hpp"
#include "SkyBox.hpp"
#include "Log.hpp"
#include "Camera.hpp"
#include "OpenGL.hpp"
#include <glm/glm.hpp>

Scene::~Scene() {}

Scene::Scene(const Camera& camera)
    : m_Camera(camera)
    , m_SkyBox(std::make_unique<SkyBox>()) {}

auto Scene::Add(GameObject&& entity) -> void
{
    m_Entities.push_back(std::move(entity));
    m_Entities.back().material()->SetTexture("uSkyboxMap", m_SkyBox->texture());
}

auto Scene::Entities() const -> std::span<const GameObject>
{
    return m_Entities;
}

auto Scene::SetSkyBox(const std::string& BasePathName) -> void
{
    m_SkyBox = std::make_unique<SkyBox>(BasePathName);
    for(const auto& e : m_Entities) e.material()->SetTexture("uSkyboxMap", m_SkyBox->texture());
}

auto Scene::SetSkyBox(std::shared_ptr<TextureCubeMap> texture) -> void
{
    m_SkyBox = std::make_unique<SkyBox>(texture);
    for(const auto& e : m_Entities) e.material()->SetTexture("uSkyboxMap", m_SkyBox->texture());
}

auto Scene::Clear() -> void
{
    m_Entities.clear();
}

auto operator<<(Scene& scene, GameObject&& entity)-> Scene&
{
    scene.Add(std::move(entity));
    return scene;
}

auto Scene::RenderSky() const -> void{

    gl::DepthFunc(GL_LEQUAL);

    m_SkyBox->Program()->Use();
    m_SkyBox->Program()->SetUniform("View", glm::mat4(glm::mat3(m_Camera.View())));
    m_SkyBox->Program()->SetUniform("Projection", m_Camera.Perspective());
    m_SkyBox->Program()->SetUniform("uDiffuseMap", m_SkyBox->texture()->TextureUnit());

    gl::BindVertexArray(m_SkyBox->mesh()->VAO);
    gl::DrawArrays(GL_TRIANGLES, 0, m_SkyBox->mesh()->VextexSize());

    gl::DepthFunc(GL_LESS);
}
