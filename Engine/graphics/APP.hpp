#pragma once

#include "Window.hpp"
#include "OpenGL.hpp"
#include "Renderer.hpp"
#include "Scene.hpp"
#include "SkyBox.hpp"

#include <core/Log.hpp>

#include <inputs/Keyboard.hpp>
#include <inputs/Mouse.hpp>

#include <ui/Text.hpp>

#include <engine_export.h>

#include <memory>

class ENGINE_EXPORT IGame {
public:
    IGame();
    virtual ~IGame();
    virtual auto update(float dt) -> void;
    virtual auto on_deltamouse(float dx, float dy) -> void;

    class APP& app;
    ::Scene Scene;
};

class ENGINE_EXPORT APP
{
    APP();
public:
    
    auto run() -> void ;
    auto fps() const -> float;
    auto render_stats() const -> RenderStats;

    static auto loop_body(void* ctx) -> void;
    static auto self(IGame* g = nullptr) -> APP&;

private:
    auto frame() -> void;

private:
    bool m_Running;
    float m_Fps;

public:
    ::CWindow Window;
    ::Keyboard Keyboard;
    ::Mouse Mouse;

private:
    OpenGL m_GApi;
    std::unique_ptr<IRenderer> Renderer;

public:
    Text UiText;
    SkyBox sKybOx;
private:
    IGame* Game;
};