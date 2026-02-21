#include "SysInfo.hpp"
#include "format.hpp"

auto os::build_info() -> const char*
{
    return "Build info: \n"
    " - host os name: " SYSTEM_HOST_NAME "\n"
    " - host os version: " SYSTEM_HOST_VERSION "\n"
    " - host os arch: " SYSTEM_HOST_ARCH "\n"
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
        case Arch::x86_x64: return "x86_x64";
        case Arch::arm :    return "arm";
        case Arch::wasm:    return "wasm";
        case Arch::unknown:
        default: return "unknown";
    }
}

auto os::arch_bits() -> std::size_t
{
    return sizeof(void*) * 8;
}
