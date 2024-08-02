#include "Utils.hpp"
#include "Window.hpp"
float MouseXFromWindowToOpengl(const Window &win)
{
    float center = win.GetWidth() / 2;
    float x = (float)win.mouse.GetPosX() - center ;

    auto MousePosClipX =  x / center;
    return MousePosClipX;
}

float MouseYFromWindowToOpengl(const Window &win)
{
    float center = win.GetHeight() / 2;
    float y = (float)win.mouse.GetPosY() - center ;

    auto MousePosClipY =  -y / center;
    return MousePosClipY;
}
