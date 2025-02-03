#pragma once
#include <glm/glm.hpp>
#include <core/Mesh.hpp>
#include <core/Transform.hpp>
#include <core/Material.hpp>
#include <core/fmts.hpp>
#include <vector>

#include <format>


class GameObject {
public:
    GameObject(glm::vec3 position, Material &matt, const Mesh &mesh);
    ~GameObject();

    auto UpMatrix()                                             -> void ;
    auto SetUp(std::vector<glm::vec3> InsPos)                   -> void ;
    auto SetPosition(const glm::vec3 &pos)                      -> void ;
    auto SetScale(const glm::vec3 &Scale)                       -> void ;
    auto Rotate(const float &x, const float &y, const float &z) -> void ;
    auto GetTransform() const                                   -> Transform ;
    auto GetModleMatrix() const                                 -> glm::mat4 ;
    auto GetMesh() const                                        -> const Mesh& ;
    auto GetMaterial() const                                    -> Material* ;
    auto GetInstancePos() const                                 -> const std::vector<glm::vec3>&;

private:
    auto Transformation() const                                 -> glm::mat4 ;

private:
    Transform transform;
    Material* material;
    Mesh m_Mesh;
    std::vector<glm::vec3> InstancePos;
};

// custom GameObject Format
template<>
struct std::formatter<GameObject> {
  constexpr auto parse(std::format_parse_context& context) {
    return context.begin();
  }
  auto format(const GameObject& obj, std::format_context& context) const {
    return std::format_to(context.out(),
    "GameObject: {{ transform: {}, material: {}, mesh: {} }}"
    , obj.GetTransform(), *obj.GetMaterial(), obj.GetMesh());
  }
};