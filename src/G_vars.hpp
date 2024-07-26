#pragma once
#include "Global_H.hpp"
#include <windows.h>
#include "Renderer.hpp"
NO_WARNING_BEGIN
#include <glad/glad.h>
NO_WARNING_END

#define WM_FPS_PAINT (WM_USER + 1)

// OpenGl Vars
inline GLfloat vertices[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };


//Debug Vars
inline LPCSTR  CLASS_NAME  = "CustomClass";
inline WNDCLASSEXA windclass = 
    {
    .cbSize = sizeof(WNDCLASSEXA),
    .style = CS_GLOBALCLASS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW| CS_DBLCLKS,
    .lpfnWndProc = nullptr,
    .cbClsExtra = 0,
    .cbWndExtra = 0,
    .hInstance =  GetModuleHandleA(0),
    .hIcon = LoadIconA(nullptr, IDI_APPLICATION),
    .hCursor = LoadCursorA(nullptr, IDC_ARROW),
    .hbrBackground = nullptr,
    .lpszMenuName = "",
    .lpszClassName = CLASS_NAME,
    .hIconSm = LoadIconA(nullptr, IDI_APPLICATION),
    };
inline LARGE_INTEGER start_count, end_count, counts, freq, fps;