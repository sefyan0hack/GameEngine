#include "res.hpp"
#include <engine_export.h>

#if defined(EMBED_RES)

#include <cmrc/cmrc.hpp>
CMRC_DECLARE(res);
static auto embed_filesystem = cmrc::res::get_filesystem();

#else

#include <fstream>
#include <vector>
#include <unordered_map>
#include "Exception.hpp"

#endif

auto res::get(const char* res_name) -> std::span<const char>
{
    #if defined(EMBED_RES)

    auto f = embed_filesystem.open(res_name);
    return std::span<const char>(f.begin(), f.end());

    #else

    static std::unordered_map<std::string, std::vector<char>> cache;

    auto& data = cache[res_name];

    if (data.empty()) {
        std::ifstream file(res_name, std::ios::binary | std::ios::ate);

        if (!file.is_open())
            throw Exception("failed to open file: {}", res_name);

        auto size = static_cast<std::size_t>(file.tellg());

        if (size < 0)
            throw Exception("failed to get file size: {}", res_name);

        file.seekg(0);

        data.resize(size);

        file.read(data.data(), static_cast<std::streamsize>(size));
    }

    return std::span<const char>(data.data(), data.size());
    
    #endif
}