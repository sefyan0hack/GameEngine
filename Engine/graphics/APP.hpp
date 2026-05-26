#pragma once

#include "Window.hpp"
#include "OpenGL.hpp"
#include "Renderer.hpp"
#include "Scene.hpp"
#include <core/Log.hpp>
#include <inputs/Keyboard.hpp>
#include <inputs/Mouse.hpp>
#include <ui/Text.hpp>
#include <engine_export.h>

#include <memory>

class ENGINE_EXPORT IGame {
public:
    virtual ~IGame() = default;
    virtual auto update(float dt) -> void { debug::log("update({})", dt); }
    virtual auto on_deltamouse(float dx, float dy) -> void { debug::log("on_deltamouse({},{})", dx, dy); }
};

class ENGINE_EXPORT APP
{
    APP();
public:
    friend class Game;
    
    auto run() -> void ;
    auto fps() const -> float;
    auto deltatime() const -> float;

    static auto loop_body(void* ctx) -> void;
    static auto self(IGame* g = nullptr) -> APP&;

private:
    auto frame() -> void;

private:
    bool m_Running;
    float m_Fps;
    uint32_t m_FrameCount;
    float m_AccumulatedTime;

    ::CWindow Window;
    ::Keyboard Keyboard;
    ::Mouse Mouse;

    OpenGL m_GApi;
    std::unique_ptr<IRenderer> Renderer;
    Text UiText;
    Scene MainScene;

    inline static IGame defaultGame;
    IGame* Game;
};