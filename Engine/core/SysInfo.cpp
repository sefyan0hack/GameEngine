#include "SysInfo.hpp"
#include "format.hpp"

auto os::build_info() -> const char*
{
    return "Build info: \n"
    " - os name: " SYSTEM_HOST_NAME "\n"
    " - os version: " SYSTEM_HOST_VERSION "\n"
    " - os arch: " SYSTEM_HOST_ARCH "\n"
    " - compiler name: " COMPILER_NAME "\n"
    " - compiler version: " COMPILER_VERSION "\n"
    " - build config: " BUILD_CONFIG "\n"
    " - std: c++" CXX_STANDARD "\n"
    " - timestamp: " __TIMESTAMP__ "\n";  
}

auto os::arch_name() -> std::string
{
    switch (arch())
    {
        case Arch::x64:
            return "x64";
        case Arch::x86:
            return "x86";
        case Arch::arm :
            return "arm";
        case Arch::arm64:
            return "arm64";
        case Arch::wasm:
            return "wasm";
        case Arch::wasm64:
            return "wasm64";
        case Arch::unknown:
        default:
            return "unknown";
    }
}