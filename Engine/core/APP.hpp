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

class ENGINE_EXPORT APP
{

public:
    using game_ctor_t = void*(*)(APP&);
    using game_dtor_t = void(*)(void*);
    using game_link_t = void(*)(void**);
    using game_update_t = void(*)(void*,float);
    using game_on_deltamouse_t = void(*)(void*,float, float);

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
    auto init_game_library() -> void;
    auto load_game_library() -> void;
    auto unload_game_library() -> void;
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

    game_ctor_t game_ctor;
    game_dtor_t game_dtor;
    game_link_t game_link;
    game_update_t game_update;
    game_on_deltamouse_t game_on_deltamouse;
};