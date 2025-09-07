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

auto Scene::add(GameObject&& entity) -> void
{
    m_Entities.push_back(std::move(entity));
    m_Entities.back().material()->set_texture("uSkyboxMap", m_SkyBox->texture());
}

auto Scene::entities() const -> std::span<const GameObject>
{
    return m_Entities;
}

auto Scene::set_skybox(const std::string& BasePathName) -> void
{
    m_SkyBox = std::make_unique<SkyBox>(BasePathName);
    for(const auto& e : m_Entities) e.material()->set_texture("uSkyboxMap", m_SkyBox->texture());
}

auto Scene::set_skybox(std::shared_ptr<TextureCubeMap> texture) -> void
{
    m_SkyBox = std::make_unique<SkyBox>(texture);
    for(const auto& e : m_Entities) e.material()->set_texture("uSkyboxMap", m_SkyBox->texture());
}

auto Scene::clear() -> void
{
    m_Entities.clear();
}

auto operator<<(Scene& scene, GameObject&& entity)-> Scene&
{
    scene.add(std::move(entity));
    return scene;
}

auto Scene::render_sky() const -> void{

    gl::DepthFunc(GL_LEQUAL);

    m_SkyBox->program()->use();
    m_SkyBox->program()->set_uniform("View", glm::mat4(glm::mat3(m_Camera.view())));
    m_SkyBox->program()->set_uniform("Projection", m_Camera.perspective());
    m_SkyBox->program()->set_uniform("uDiffuseMap", m_SkyBox->texture()->texture_unit());

    gl::BindVertexArray(m_SkyBox->mesh()->VAO);
    gl::DrawArrays(GL_TRIANGLES, 0, m_SkyBox->mesh()->vextex_size());

    gl::DepthFunc(GL_LESS);
}
