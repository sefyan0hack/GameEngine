#include "SysInfo.hpp"
#include <emscripten.h>
#include <emscripten/heap.h>
#include <unistd.h>
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>

#include "Exception.hpp"

auto os::system() -> os::Target
{
    return os::Target::Web;
}

auto os::system_name() -> std::string
{
    return "web";
}

auto os::arch() -> os::Arch
{
    if constexpr(sizeof(void*) == 8) return os::Arch::wasm64;
    else return os::Arch::wasm;
}