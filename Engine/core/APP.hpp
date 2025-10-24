#pragma once

#include <memory>
#include <chrono>

#include "Window.hpp"
#include "Camera.hpp"
#include <input/Keyboard.hpp>
#include <input/Mouse.hpp>
#include "EventQueue.hpp"
#include <graphics/Renderer.hpp>

class APP
{
protected:

    APP();
    ~APP();

    virtual auto update(float delta) -> void = 0 ;
public:
    auto run() -> void ;
    auto fps() const -> float;
    auto smooth_fps() const -> float;
    auto deltatime() const -> float;

    auto push_event(const Event& event) -> void;

    auto render(const class Scene& scene, std::shared_ptr<ShaderProgram> program) -> void;

private:
    auto pull_event(Event& event) -> bool;
    auto wait_event(Event& event) -> void;
    auto clear_events() -> void;

    auto frame(float deltaTime) -> void;
    static auto loop_body(void* ctx) -> void;

protected:
    CWindow Window;
    Camera ViewCamera;
    Keyboard Keyboard;
    Mouse Mouse;

private:
    Renderer m_Renderer;

private:
    bool m_Running;
    EventQueue ApplicationEventQueue;
    std::chrono::steady_clock::time_point m_LastFrameTime;
    float m_Fps, m_SmoothedFPS;
    
};