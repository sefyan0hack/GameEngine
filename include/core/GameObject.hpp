#pragma once
#include <glm/glm.hpp>
#include <core/Transform.hpp>

class GameObject {
public:
    friend struct std::formatter<GameObject>;
    GameObject(glm::vec3 position, std::shared_ptr<class Material> matt, std::shared_ptr<class Mesh> mesh, std::string Name = std::format("Obj{}", Count));
    GameObject(Transform transform, std::shared_ptr<class Material> matt, std::shared_ptr<class Mesh> mesh, std::string Name = std::format("Obj{}", Count));
    ~GameObject();

    GameObject(const GameObject&) = default;
    auto operator=(const GameObject&) -> GameObject& = default;

    GameObject(GameObject&& other) noexcept;
    auto operator=(GameObject&& other) noexcept -> GameObject&;

    auto SetPosition(const glm::vec3 &pos)                   -> void ;
    auto SetScale(const glm::vec3 &Scale)                    -> void ;
    auto Rotate(float angle, glm::vec3 axis)                 -> void ;
    auto transform() const                                   -> Transform ;
    auto Model() const                                       -> glm::mat4 ;
    auto mesh() const                                        -> std::shared_ptr<class Mesh> ;
    auto material() const                                    -> std::shared_ptr<class Material> ;
    auto Bind() const                                        -> void;
    auto Name() const                                        -> std::string;

private:
    Transform m_Transform;
    std::shared_ptr<class Material> m_Material;
    std::shared_ptr<class Mesh> m_Mesh;
    std::string m_Name;
    inline static std::size_t Count = 1;

    FOR_TEST
};

// custom GameObject Format
template<>
struct std::formatter<GameObject> {
  constexpr auto parse(std::format_parse_context& context) {
    return context.begin();
  }
  auto format(const GameObject& obj, std::format_context& context) const {
    return std::format_to(context.out(),
    R"({{ "name": "{}", "transform": {} }})"
    , obj.m_Name, obj.m_Transform);
  }
};