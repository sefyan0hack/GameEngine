#pragma once
#include <core/Window.hpp>
#include <core/Camera.hpp>
#include <core/Keyboard.hpp>
#include <core/Mouse.hpp>
#include <core/EventQueue.hpp>

class APP
{
protected:

    APP();
    ~APP();

    virtual auto Update(float delta) -> void = 0 ;
public:
    auto Run() -> void ;
    auto Fps() const -> float;
    auto SmoothedFPS() const -> float;
    auto DeltaTime() const -> float;

    auto PushEvent(const Event& event) -> void;

private:
    auto PollEvent(Event& event) -> bool;
    auto WaitEvent(Event& event) -> void;
    auto ClearEvents() -> void;

    auto Frame(float deltaTime) -> void;
    static auto LoopBody(void* ctx) -> void;
protected:
    CWindow Window;
    Camera ViewCamera;
    Keyboard Keyboard;
    Mouse Mouse;
private:
    bool m_Running;
    EventQueue ApplicationEventQueue;
    std::chrono::steady_clock::time_point m_LastFrameTime;
    float m_Fps, m_SmoothedFPS;

    FOR_TEST
};