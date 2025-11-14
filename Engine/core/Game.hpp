#pragma once
#include <memory>
#include <engine_export.h>

class ENGINE_API IGame {
public:
    virtual ~IGame() = default;
    virtual auto update(float delta) -> void = 0;
    virtual auto on_deltamouse(float dx, float dy) -> void = 0;
};