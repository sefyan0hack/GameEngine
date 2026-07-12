#pragma once
#include <android/log.h>

inline auto plt_out(const char* str) -> void
{
    __android_log_print(ANDROID_LOG_INFO, "ENGINE", "%s", str);
}