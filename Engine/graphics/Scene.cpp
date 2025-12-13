#include "Scene.hpp"
#include "Camera.hpp"
#include "GameObject.hpp"
#include "SkyBox.hpp"
#include "Texture.hpp"
#include "Material.hpp"
#include "ShaderProgram.hpp"
#include "Mesh.hpp"
#include "OpenGL.hpp"
#include <core/Log.hpp>
#include <glm/glm.hpp>


Scene::Scene()
    : m_Cameras({Camera()})
    , m_MainCamera(m_Cameras[0])
    , m_SkyBox(std::make_unique<SkyBox>()) { CTOR_LOG }

Scene::~Scene() = default;
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

auto Scene::operator<<(GameObject entity)-> Scene&
{
    entity.material()->set_texture("uSkyboxMap", m_SkyBox->texture());
    m_Entities.emplace_back(std::move(entity));
    return *this;
}

auto Scene::operator<<(Camera cam)-> Scene&
{
    m_Cameras.emplace_back(std::move(cam));
    return *this;
}

auto Scene::render_sky() const -> void{

    gl::DepthFunc(GL_LEQUAL);

    m_SkyBox->program()->use();
    m_SkyBox->program()->set_uniform("View", glm::mat4(glm::mat3(m_MainCamera.view())));
    m_SkyBox->program()->set_uniform("Projection", m_MainCamera.perspective());
    m_SkyBox->program()->set_uniform("uDiffuseMap", m_SkyBox->texture()->texture_unit());

    gl::BindVertexArray(m_SkyBox->mesh()->VAO);
    gl::DrawArrays(GL_TRIANGLES, 0, m_SkyBox->mesh()->vextex_size());

    gl::DepthFunc(GL_LESS);
}

auto Scene::main_camera() -> Camera&
{
    return m_MainCamera;
}
auto Scene::main_camera() const -> Camera&
{
    return m_MainCamera;
}