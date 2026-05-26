#pragma once
#include <span>

namespace res
{
    auto get(const char* res_name) -> std::span<const char>;
}