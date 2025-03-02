#pragma once

#include <vector>
#include <memory>

class GameObject;
class SkyBox;
class Scene
{
public:
    Scene() = default;
    Scene(std::unique_ptr<SkyBox> skybox);
    ~Scene();
    auto operator << (GameObject obj);

    auto add(GameObject gobj)   -> void;
    auto Entitys()       -> std::vector<GameObject>&;
    auto setSkyBox(std::unique_ptr<SkyBox> skybox) -> void;
    auto skyBox() -> std::unique_ptr<SkyBox>&;
private:
    std::unique_ptr<SkyBox> m_SkyBox;
    std::vector<GameObject> m_Entitys;
};