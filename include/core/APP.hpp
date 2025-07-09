#pragma once
#include <core/Window.hpp>

class APP
{
protected:

    APP();
    ~APP() = default;

    virtual auto Update(float delta) -> void = 0 ;
public:
    auto Run() -> void ;
    auto Fps() const -> float;
    auto SmoothedFPS() const -> float;
    auto DeltaTime() const -> float;

private:
    auto Frame(float deltaTime) -> void;
    static auto LoopBody(void* ctx) -> void;
protected:
    CWindow m_Window;
    Keyboard m_Keyboard;
    Mouse m_Mouse;
    std::chrono::steady_clock::time_point m_LastFrameTime;
    float m_Fps, m_SmoothedFPS;

    FOR_TEST
};