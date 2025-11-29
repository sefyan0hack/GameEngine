#pragma once

#include <memory>
#include <chrono>

#include <graphics/Window.hpp>
#include <graphics/Renderer.hpp>
#include <input/Keyboard.hpp>
#include <input/Mouse.hpp>
#include "DynLib.hpp"
#include "EventQueue.hpp"
#include <engine_export.h>

#define GAME_API \
    X(game_ctor,            void*, (APP&))\
    X(game_dtor,            void, (void*))\
    X(game_link,            void, (void**))\
    X(game_update,          void, (void*,float))\
    X(game_on_deltamouse,   void, (void*,float, float))


class ENGINE_EXPORT APP
{

public:
    #undef X
    #define X(name, r, args) using name##_t = r(*)args;

    GAME_API

public:
    friend class Game;

    APP();
    ~APP();

    auto run() -> void ;
    auto fps() const -> float;
    auto deltatime() const -> float;
    auto push_event(const Event& event) -> void;

    auto render(const class Scene& scene, std::shared_ptr<ShaderProgram> program) -> void;

private:
    auto init_game_functions() -> void;
    auto hot_reload_game_library() -> bool;

    auto pull_event(Event& event) -> bool;
    auto wait_event(Event& event) -> void;

    auto frame(float deltaTime) -> void;
    static auto loop_body(void* ctx) -> void;

private:
    CWindow Window;
    Keyboard Keyboard;
    Mouse Mouse;
    
    bool m_Running;
    EventQueue ApplicationEventQueue;
    std::chrono::steady_clock::time_point m_LastFrameTime;
    float m_Fps;

    IRenderer* Renderer;

    DynLib lib;
    void* Game;

    #undef X
    #define X(name, r, args) name##_t name;

    GAME_API;
};