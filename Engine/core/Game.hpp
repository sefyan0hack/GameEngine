#pragma once
class IGame {
public:
    virtual ~IGame() = default;
    virtual auto update(float delta) -> void = 0;
    virtual auto on_deltamouse(float dx, float dy) -> void = 0;
};