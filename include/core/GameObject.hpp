#pragma once
#include <glm/glm.hpp>
#include <core/Mesh.hpp>
#include <core/Transform.hpp>
#include <core/Material.hpp>
#include <core/fmts.hpp>
#include <core/AutoRelease.hpp>
#include <vector>
#include <memory>

#include <format>


class GameObject {
public:
    friend struct std::formatter<GameObject>;
    GameObject(glm::vec3 position, Material& matt, Mesh& mesh, std::string Name = std::format("Obj{}", Count));
    GameObject(Transform transform, Material& matt, Mesh& mesh, std::string Name = std::format("Obj{}", Count));
    ~GameObject();

    auto UpMatrix()                                             -> void ;
    auto SetUp(std::vector<glm::vec3> InsPos)                   -> void ;
    auto SetPosition(const glm::vec3 &pos)                      -> void ;
    auto SetScale(const glm::vec3 &Scale)                       -> void ;
    auto Rotate(const float &x, const float &y, const float &z) -> void ;
    auto transform() const                                   -> Transform ;
    auto Modle() const                                 -> glm::mat4 ;
    auto mesh() const                                        -> std::shared_ptr<Mesh> ;
    auto material() const                                    -> std::shared_ptr<Material> ;
    auto InstancePos() const                                 -> const std::vector<glm::vec3>&;
    auto Bind() const                                           -> void;

private:
    auto Transformation() const                                 -> glm::mat4 ;
    private:
    Transform m_Transform;
    std::shared_ptr<Material> m_Material;
    std::shared_ptr<Mesh> m_Mesh;
    std::vector<glm::vec3> m_InstancePos;
    std::string m_Name;
    inline static size_t Count = 1;
};

// custom GameObject Format
template<>
struct std::formatter<GameObject> {
  constexpr auto parse(std::format_parse_context& context) {
    return context.begin();
  }
  auto format(const GameObject& obj, std::format_context& context) const {
    return std::format_to(context.out(),
    "{}: {{ name: {}, transform: {}, material: {}, mesh: {} }}"
    , typeid(obj).name(), obj.m_Name, obj.transform(), *obj.m_Material, *obj.m_Mesh.get());
  }
};