#include <core/Utils.hpp>
#include <string>
#include <cmath>
#include <print>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {

    const char* string = (const char*)Data;
    std::string delim = (char)(std::rand() % 127);

    std::println("sting: {}", string);
    std::println("delim: {}", delim);
    std::println("result: {}", utils::split(string, delim));

    return 0;
}