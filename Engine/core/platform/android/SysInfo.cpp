#include "SysInfo.hpp"
#include "Log.hpp"
#include "Exception.hpp"
#include <fstream>
#include <string>
#include <sstream>
#include <cassert>
#include <unistd.h>
#include <dirent.h>
#include <sys/system_properties.h>
#include <dlfcn.h>

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
            assert(false && "wasm not valind in this context");
        case Arch::wasm64:
            assert(false && "wasm64 not valind in this context");
        case Arch::unknown:
        default:
            return "unknown";
    }
}

auto os::proc_id() -> std::size_t
{
    return static_cast<std::size_t>(getpid());
}

static std::size_t get_proc_value(const std::string& key)
{
    std::ifstream stream("/proc/self/status");
    std::string line;
    while (std::getline(stream, line)) {
        if (line.find(key) == 0) {
            std::stringstream ss(line);
            std::string label;
            std::size_t value;
            ss >> label >> value;
            return value;
        }
    }
    return 0;
}

auto os::memory_usage() -> std::size_t
{
    return get_proc_value("VmRSS:") / 1024;
}

auto os::memory_peak() -> std::size_t
{
    return get_proc_value("VmHWM:") / 1024;
}

auto os::thread_count() -> std::size_t
{
    return get_proc_value("Threads:");
}

auto os::get_proc_address(const char* module, const char* sym) -> void* {

    void* lib = nullptr;
    void* address = nullptr;
    std::string failreson;

    lib = dlopen(module, RTLD_LAZY | RTLD_NOLOAD);
    failreson = lib ? "" : std::string(dlerror());
    address = (void *)dlsym(lib, sym);

    if(lib == nullptr){
        throw Exception("Couldn't load lib {} reason: {}, fn name: {}", module, failreson, sym);
    }

    if(address == nullptr){
        throw Exception("Couldn't load symbole {} reason: {}", sym, failreson);
    }

    return address;
}