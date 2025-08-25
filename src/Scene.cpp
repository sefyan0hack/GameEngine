#include <core/Scene.hpp>
#include <core/Texture.hpp>
#include <core/Material.hpp>
#include <core/GameObject.hpp>
#include <core/ShaderProgram.hpp>
#include <core/Mesh.hpp>
#include <core/SkyBox.hpp>
#include <core/Log.hpp>
#include <core/Camera.hpp>
#include <core/OpenGL.hpp>
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
    gl::FrontFace(GL_CW);

    m_SkyBox->Program()->Use();
    m_SkyBox->Program()->SetUniform("View", glm::mat4(glm::mat3(m_Camera.View())));
    m_SkyBox->Program()->SetUniform("Projection", m_Camera.Perspective());
    m_SkyBox->Program()->SetUniform("uDiffuseMap", m_SkyBox->texture()->TextureUnit());

    gl::BindVertexArray(m_SkyBox->mesh()->VAO);
    gl::DrawArrays(GL_TRIANGLES, 0, m_SkyBox->mesh()->VextexSize());

    gl::FrontFace(GL_CCW);
    gl::DepthFunc(GL_LESS);
}
