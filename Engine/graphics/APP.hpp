#pragma once

#include "Window.hpp"
#include "OpenGL.hpp"
#include "Renderer.hpp"
#include "Scene.hpp"
#include <inputs/Keyboard.hpp>
#include <inputs/Mouse.hpp>
#include <ui/Text.hpp>
#include <engine_export.h>

class ENGINE_EXPORT IGame {
public:
    virtual ~IGame() = default;
    virtual auto update(float dt) -> void = 0;
    virtual auto on_deltamouse(float, float) -> void = 0;
};

class ENGINE_EXPORT APP
{
    APP();
    ~APP();
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
    IRenderer* Renderer;
    Text UiText;
    Scene MainScene;

    IGame* Game;
};