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
    int64_t start_count = 0, end_count = 0, counts = 0, freq = 0, fps = 60;

    FOR_TEST
};