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

protected:
    Window m_Window;
    LARGE_INTEGER start_count, end_count, counts, freq, fps;
};