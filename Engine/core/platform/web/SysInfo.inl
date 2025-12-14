#include <emscripten.h>
#include <emscripten/heap.h>
#include <unistd.h>

auto os::host::name_tag() -> os::Target
{
    return os::Target::Web;
}
    
auto os::host::name() -> std::string
{
    return "web";
}

auto os::host::arch_tag() -> os::Arch
{
    return os::build::arch_tag();
}

auto os::host::arch() -> std::string
{
    return os::build::arch();
}

auto os::host::proc_id() -> std::size_t
{
    // Emscripten provides a shim for getpid() which returns the process ID
    // In a WASM/browser context, this ID is typically fixed (1)
    // or represents the main worker thread ID
    return static_cast<std::size_t>(getpid());
}

auto os::host::memory_usage() -> std::size_t
{
    return emscripten_get_heap_size() / 1024 / 1024; // MB
}

auto os::host::memory_peak() -> std::size_t
{
    return emscripten_get_heap_max() / 1024 / 1024; // MB
}

auto os::host::thread_count() -> std::size_t
{
    return (std::size_t)EM_ASM_INT({
        if (typeof PThread !== 'undefined' && PThread.pthreads) {
            return Object.keys(PThread.pthreads).length;
        }
        return 1;
    });
}

auto os::host::module() -> std::string
{
    return "";
}