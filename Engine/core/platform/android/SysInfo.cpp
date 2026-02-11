#include "SysInfo.hpp"
#include <string>
#include <sys/system_properties.h>

auto os::system() -> os::Target
{
    return os::Target::Android;
}
 
auto os::system_name() -> std::string
{
    return "Android";
}

auto os::arch() -> os::Arch
{
    char value[PROP_VALUE_MAX];
    int len = __system_property_get("ro.product.cpu.abi", value);
    
    if (len <= 0) return os::Arch::unknown;

    std::string abi(value);
    if (abi == "arm64-v8a") return os::Arch::arm64;
    if (abi == "armeabi-v7a" || abi == "armeabi") return os::Arch::arm;
    if (abi == "x86_64") return os::Arch::x64;
    if (abi == "x86") return os::Arch::x86;

    return os::Arch::unknown;
}