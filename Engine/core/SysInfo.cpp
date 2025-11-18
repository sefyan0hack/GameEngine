#include "SysInfo.hpp"

#if defined(WINDOWS_PLT)
    #include <windows.h>
#elif defined(LINUX_PLT)
    #include <sys/utsname.h>
#endif

#if defined(WINDOWS_PLT)
#	include "platform/windows/SysInfo.inl"
#elif defined(LINUX_PLT)
#	include "platform/linux/SysInfo.inl"
#elif defined(WEB_PLT)
#	include "platform/web/SysInfo.inl"
#endif

constexpr auto sys::build::name() -> sys::Target
{
    #if defined(WINDOWS_PLT)
        return sys::Target::Windows;
    #elif defined(LINUX_PLT)
        return sys::Target::Linux;
    #elif defined(WEB_PLT)
        return sys::Target::Web;
    #else
        return sys::Target::unknown;
    #endif
}
constexpr auto sys::build::name_str() -> std::string
{
    return 
    name() == sys::Target::Windows ? "windows" :
    name() == sys::Target::Linux   ? "linux"   :
    name() == sys::Target::Web     ? "web"     : "unknown";
}
 
constexpr auto sys::build::arch() -> sys::Arch
{
    #if   defined(__x86_64__)  || defined(_M_AMD64)
        return sys::Arch::x64;
    #elif defined(__i386__)    || defined(_M_IX86)
        return sys::Arch::x86;
    #elif defined(__arm__)     || defined(_M_ARM)
        return sys::Arch::arm;
    #elif defined(__aarch64__) || defined(_M_ARM64)
        return sys::Arch::arm64;
    #elif defined(__wasm32__)
        return sys::Arch::wasm;
    #elif defined(__wasm64__)
        return sys::Arch::wasm64;
    #else
        return sys::Arch::unknown;
    #endif
}
    
constexpr auto sys::build::arch_str() -> std::string
{
    return
    arch() == sys::Arch::x64    ? "x64"    :
    arch() == sys::Arch::x86    ? "x86"    :
    arch() == sys::Arch::arm    ? "arm"    :
    arch() == sys::Arch::arm64  ? "arm64"  :
    arch() == sys::Arch::wasm   ? "wasm"   :
    arch() == sys::Arch::wasm64 ? "wasm64" : "unknown";
}