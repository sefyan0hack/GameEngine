#pragma once

#include <vector>

class GameObject;
class Scene
{
public:
    Scene(/* args */);
    ~Scene();

    auto add(const GameObject &gobj)   -> void;
    auto GetGameObjects()       -> std::vector<GameObject>&;
private:
    std::vector<GameObject> objs;
};