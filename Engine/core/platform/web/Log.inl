#pragma once
#include <cstdio>

inline auto printf_(const char* fmt, auto... args) -> int
{
    return printf(fmt, args...);
}