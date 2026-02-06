#include <core/Utils.hpp>
#include <string>
#include <cmath>
#include <print>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    if (Size == 0) return 0;

    std::string string{reinterpret_cast<const char*>(Data), Size};
    std::string delim = {(char)(32 + std::rand() % 127)};

    utils::split(string.c_str(), delim);

    return 0;
}