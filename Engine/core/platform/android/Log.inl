#pragma once
#include <android/log.h>

inline auto printf_(const char* fmt, auto... args) -> int
{
    __android_log_print(ANDROID_LOG_INFO, "ENGINE", fmt, args...);
}