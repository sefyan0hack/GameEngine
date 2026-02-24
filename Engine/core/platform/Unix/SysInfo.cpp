#include "SysInfo.hpp"
#include "Exception.hpp"
#include <fstream>
#include <string>
#include <sstream>
#include <unistd.h>
#include <dirent.h>
#include <dlfcn.h>

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