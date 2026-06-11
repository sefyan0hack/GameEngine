#include "res.hpp"
#include "Exception.hpp"
#include <cmrc/cmrc.hpp>
#include <engine_export.h>

CMRC_DECLARE(res);

auto res::get(const char* res_name) -> std::span<const char>
{
    auto f = cmrc::res::get_filesystem().open(res_name);
    return std::span<const char>(f.begin(), f.end());
}