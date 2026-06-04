#include "res.hpp"
#include "Exception.hpp"
#include <cmrc/cmrc.hpp>
#include <engine_export.h>

CMRC_DECLARE(res);
static auto embed_filesystem = cmrc::res::get_filesystem();

auto res::get(const char* res_name) -> std::span<const char>
{
    if (!embed_filesystem.exists(res_name))
        throw Exception("Embedded resource not found: {}", res_name);

    auto f = embed_filesystem.open(res_name);
    return std::span<const char>(f.begin(), f.end());
}