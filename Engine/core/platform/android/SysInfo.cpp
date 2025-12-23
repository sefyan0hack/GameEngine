#include "SysInfo.hpp"
#include "Log.hpp"
#include "Exception.hpp"
#include <fstream>
#include <string>
#include <sstream>
#include <cassert>

auto os::host::name_tag() -> os::Target
{
    return os::Target::Android;
}
    
auto os::host::name() -> std::string
{
    return "Android";
}

auto os::host::arch_tag() -> os::Arch
{
    unimpl();
}

auto os::host::arch() -> std::string
{
    // arm64-v8a
    // armeabi
    // armeabi-v7a
    // mips64
    // mips
    // x86_64
    // x86
    switch (arch_tag())
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

auto os::host::proc_id() -> std::size_t
{
    unimpl();
}

static std::size_t get_proc_value(const std::string& key)
{
    unimpl();
}

auto os::host::memory_usage() -> std::size_t
{
    unimpl();
}

auto os::host::memory_peak() -> std::size_t
{
    unimpl();
}

auto os::host::thread_count() -> std::size_t
{
    unimpl();
}

auto os::host::module() -> std::string
{
    unimpl();
}