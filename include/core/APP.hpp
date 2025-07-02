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
    #if defined(WEB_PLT)
    static auto WebLoop(void* userData) -> void;
    #endif
protected:
    CWindow m_Window;
    double fps;

    FOR_TEST
};