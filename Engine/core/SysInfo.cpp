#include "SysInfo.hpp"


constexpr auto os::build::name_tag() -> os::Target
{
    #if defined(WINDOWS_PLT)
        return os::Target::Windows;
    #elif defined(LINUX_PLT)
        return os::Target::Linux;
    #elif defined(WEB_PLT)
        return os::Target::Web;
    #else
        return os::Target::unknown;
    #endif
}
constexpr auto os::build::name() -> std::string
{
    return 
    name_tag() == os::Target::Windows ? "windows" :
    name_tag() == os::Target::Linux   ? "linux"   :
    name_tag() == os::Target::Web     ? "web"     : "unknown";
}

constexpr auto os::build::arch_tag() -> os::Arch
{
    #if   defined(__x86_64__)  || defined(_M_AMD64)
        return os::Arch::x64;
    #elif defined(__i386__)    || defined(_M_IX86)
        return os::Arch::x86;
    #elif defined(__arm__)     || defined(_M_ARM)
        return os::Arch::arm;
    #elif defined(__aarch64__) || defined(_M_ARM64)
        return os::Arch::arm64;
    #elif defined(__wasm32__)
        return os::Arch::wasm;
    #elif defined(__wasm64__)
        return os::Arch::wasm64;
    #else
        return os::Arch::unknown;
    #endif
}
    
constexpr auto os::build::arch() -> std::string
{
    return
    arch_tag() == os::Arch::x64    ? "x64"    :
    arch_tag() == os::Arch::x86    ? "x86"    :
    arch_tag() == os::Arch::arm    ? "arm"    :
    arch_tag() == os::Arch::arm64  ? "arm64"  :
    arch_tag() == os::Arch::wasm   ? "wasm"   :
    arch_tag() == os::Arch::wasm64 ? "wasm64" : "unknown";
}