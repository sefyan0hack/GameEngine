#pragma once
#include <core/Window.hpp>
#include <cstdint>

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
    int64_t start_count, end_count, counts, freq, fps;
};