#include "res.hpp"
#include "Exception.hpp"
#include <cmrc/cmrc.hpp>
#include <engine_export.h>
#include <cstdio>

CMRC_DECLARE(res);
static auto embed_filesystem = cmrc::res::get_filesystem();

auto res::get(const char* res_name) -> std::span<const char>
{
    std::string path = "res";
    auto dir_iter = embed_filesystem.iterate_directory(path);
    for (const auto& entry : dir_iter) {
        std::string full = path.empty() ? entry.filename() : path + "/" + entry.filename();
        if (entry.is_file()) {
            std::printf("[FILE] %s\n", full.c_str());
        }
    }

    auto f = embed_filesystem.open(res_name);
    return std::span<const char>(f.begin(), f.end());
}