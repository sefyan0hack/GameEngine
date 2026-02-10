#include "SysInfo.hpp"
#include "format.hpp"

auto os::build_info() -> const char*
{
    return "Build info: \n"
    " - system_name: " SYSTEM_HOST_NAME "\n"
    " - system_version: " SYSTEM_HOST_VERSION "\n"
    " - system_arch: " SYSTEM_HOST_ARCH "\n"
    " - compiler_name: " COMPILER_NAME "\n"
    " - compiler_version: " COMPILER_VERSION "\n"
    " - cxx_std: " CXX_STANDARD "\n"
    " - timestamp: " __TIMESTAMP__ "\n";  
}

auto os::arch_name() -> std::string
{
    switch (arch())
    {
        case Arch::x64:
            return "6x4";
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