#pragma once
#include "windows.h"
#include "Window.hpp"

class APP
{
public:
    APP();
    ~APP() = default;
public:
    virtual void Run();
    virtual void Start() = 0;
    virtual void Update(float delta) = 0;
    virtual void Destroy() = 0;
private:
    Window m_Window;
protected:
    LARGE_INTEGER start_count, end_count, counts, freq, fps;
};