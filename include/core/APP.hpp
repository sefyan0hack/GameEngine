#pragma once
#ifdef _WIN32
#include <core/Window.hpp>
#elif __linux__
#include <core/XWindow.hpp>
#endif //_WIN32

class APP
{
protected:
    APP();
    ~APP() = default;

    virtual auto Update(float delta) -> void = 0 ;
public:
    auto Run() -> void ;

protected:
    CWindow m_Window;
    int64_t start_count, end_count, counts, freq, fps;
};