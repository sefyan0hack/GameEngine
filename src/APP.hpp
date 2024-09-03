#pragma once
#include "windows.h"
#include "Window.hpp"

class APP
{
protected:
    APP();
    ~APP() = default;


    virtual void Update(float delta) = 0;
public:
    void Run();

protected:
    Window m_Window;
    LARGE_INTEGER start_count, end_count, counts, freq, fps;
};