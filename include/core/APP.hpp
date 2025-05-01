#pragma once
#include <core/Window.hpp>

class APP
{
protected:
    FOR_TEST(APP)

    APP();
    ~APP() = default;

    virtual auto Update(float delta) -> void = 0 ;
public:
    auto Run() -> void ;

protected:
    CWindow m_Window;
    int64_t start_count, end_count, counts, freq, fps;
};