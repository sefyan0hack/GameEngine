#pragma once

#include <graphics/Window.hpp>
#include <graphics/OpenGL.hpp>
#include <graphics/Renderer.hpp>
#include <inputs/Keyboard.hpp>
#include <inputs/Mouse.hpp>
#include <ui/Text.hpp>
#include "Scene.hpp"
#include <engine_export.h>

class ENGINE_EXPORT APP
{
public:
    friend class Game;

    APP();
    ~APP();

    auto run() -> void ;
    auto fps() const -> float;
    auto deltatime() const -> float;

    static auto loop_body(void* ctx) -> void;

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

    void* Game;
};