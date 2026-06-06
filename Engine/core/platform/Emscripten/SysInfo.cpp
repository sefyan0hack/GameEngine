#include "../Unix/SysInfo.cpp"

auto os::memory_usage() -> std::size_t
{
    return emscripten_get_heap_size() / 1024 / 1024;
}

auto os::memory_peak() -> std::size_t
{
    return emscripten_get_heap_max() / 1024 / 1024;
}