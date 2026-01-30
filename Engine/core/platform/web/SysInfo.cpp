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

auto os::arch_name() -> std::string
{
    if(arch() == os::Arch::wasm64) return "wasm64";
    else return "wasm";
}

auto os::proc_id() -> std::size_t
{
    // Emscripten provides a shim for getpid() which returns the process ID
    // In a WASM/browser context, this ID is typically fixed (1)
    // or represents the main worker thread ID
    return static_cast<std::size_t>(getpid());
}

auto os::memory_usage() -> std::size_t
{
    return emscripten_get_heap_size() / 1024 / 1024; // MB
}

auto os::memory_peak() -> std::size_t
{
    return emscripten_get_heap_max() / 1024 / 1024; // MB
}

auto os::thread_count() -> std::size_t
{
    return (std::size_t)EM_ASM_INT({
        if (typeof PThread !== 'undefined' && PThread.pthreads) {
            return Object.keys(PThread.pthreads).length;
        }
        return 1;
    });
}

auto os::get_proc_address(const char* module, const char* sym) -> void* {

    void* lib = nullptr;
    void* address = nullptr;
    std::string failreson;

    address = reinterpret_cast<void*>(emscripten_webgl_get_proc_address(sym));
    failreson = lib ? "" : reinterpret_cast<const char*>(emscripten_webgl_get_proc_address(sym));

    if(lib == nullptr){
        throw Exception("Couldn't load lib {} reason: {}, fn name: {}", module, failreson, sym);
    }

    if(address == nullptr){
        throw Exception("Couldn't load symbole {} reason: {}", sym, failreson);
    }

    return address;
}