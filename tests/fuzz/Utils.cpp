#include <core/Utils.hpp>
#include <string>
#include <cmath>
#include <print>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    if (Size == 0)
        return 0;

    std::string string{
        reinterpret_cast<const char*>(Data),
        Size
    };

    std::string delim = {(char)(std::rand() % 127)};

    std::println("sting: {}", string);
    std::println("delim: {}", delim);
    
    std::println("result: ");
    for( auto e : utils::split(string, delim)){
        std::println("{} |", e);
    }

    return 0;
}