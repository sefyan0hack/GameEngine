#include "res.hpp"
#include "Exception.hpp"
#include <cmrc/cmrc.hpp>
#include <engine_export.h>
#include <cstdio>
#include <string>
CMRC_DECLARE(res);
static auto embed_filesystem = cmrc::res::get_filesystem();

void print_all_files(const std::string& path = "") {
    auto dir_iter = embed_filesystem.iterate_directory(path);
    for (const auto& entry : dir_iter) {
        std::string full = path.empty() ? entry.filename() : path + "/" + entry.filename();
        if (entry.is_file()) {
            std::printf("[FILE] %s\n", full.c_str());
        } else if (entry.is_directory()) {
            std::printf("[DIR]  %s\n", full.c_str());
            print_all_files(full);
        }
    }
}

auto res::get(const char* res_name) -> std::span<const char>
{
    print_all_files();
    std::printf("-------------------------------\n");
    auto f = embed_filesystem.open(res_name);
    return std::span<const char>(f.begin(), f.end());
}