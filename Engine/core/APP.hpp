#pragma once

#include <memory>
#include <chrono>

#include <graphics/Window.hpp>
#include <graphics/Renderer.hpp>
#include <input/Keyboard.hpp>
#include <input/Mouse.hpp>
#include "Game.hpp"
#include "DynLib.hpp"
#include "EventQueue.hpp"
#include <engine_export.h>

class ENGINE_API APP
{

public:
    friend class Game;

    APP();
    ~APP();

    auto run() -> void ;
    auto fps() const -> float;
    auto smooth_fps() const -> float;
    auto deltatime() const -> float;
    auto push_event(const Event& event) -> void;

    auto render(const class Scene& scene, std::shared_ptr<ShaderProgram> program) -> void;

private:
    auto load_game_library() -> void;
    auto unload_game_library() -> void;
    auto hot_reload_game_library() -> bool;

    auto pull_event(Event& event) -> bool;
    auto wait_event(Event& event) -> void;
    auto clear_events() -> void;

    auto frame(float deltaTime) -> void;
    static auto loop_body(void* ctx) -> void;

private:
    CWindow Window;
    Keyboard Keyboard;
    Mouse Mouse;

    
    bool m_Running;
    EventQueue ApplicationEventQueue;
    std::chrono::steady_clock::time_point m_LastFrameTime;
    float m_Fps, m_SmoothedFPS;

    IRenderer* Renderer;
    DynLib lib;
    IGame* Game;
};