#pragma once
#include <glm/glm.hpp>
#include <core/Mesh.hpp>
#include <core/Transform.hpp>
#include <core/Material.hpp>
#include <core/fmts.hpp>
#include <vector>
#include <memory>

#include <format>


class GameObject {
public:
    friend struct std::formatter<GameObject>;
    GameObject(glm::vec3 position, Material &matt, const Mesh &mesh, std::string Name = std::format("Mesh{}", Count));
    ~GameObject();

    auto UpMatrix()                                             -> void ;
    auto SetUp(std::vector<glm::vec3> InsPos)                   -> void ;
    auto SetPosition(const glm::vec3 &pos)                      -> void ;
    auto SetScale(const glm::vec3 &Scale)                       -> void ;
    auto Rotate(const float &x, const float &y, const float &z) -> void ;
    auto GetTransform() const                                   -> Transform ;
    auto GetModleMatrix() const                                 -> glm::mat4 ;
    auto GetMesh() const                                        -> const Mesh& ;
    auto GetMaterial() const                                    -> std::shared_ptr<Material> ;
    auto GetInstancePos() const                                 -> const std::vector<glm::vec3>&;
    auto Bind() const                                           -> void;

private:
    auto Transformation() const                                 -> glm::mat4 ;
    private:
    Transform transform;
    std::shared_ptr<Material> material;
    Mesh m_Mesh;
    std::vector<glm::vec3> InstancePos;
public:
    std::string name;
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
    "GameObject: {{ name: {}, transform: {}, material: {}, mesh: {} }}"
    , obj.name, obj.GetTransform(), *obj.material, obj.m_Mesh);
  }
};