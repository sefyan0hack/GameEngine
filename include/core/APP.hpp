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
private:
    auto Frame() -> void;
    static auto LoopBody(void* ctx) -> void;
protected:
    CWindow m_Window;
    float fps;

    FOR_TEST
};