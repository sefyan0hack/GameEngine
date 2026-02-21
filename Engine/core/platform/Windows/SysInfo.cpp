#include "SysInfo.hpp"
#include "Log.hpp"
#include "Exception.hpp"
#include <windows.h>
#include <psapi.h>
#include <tlhelp32.h>
#include <string>


auto os::proc_id() -> size_t
{
    return static_cast<size_t>(GetCurrentProcessId());
}

auto os::memory_usage() -> size_t
{
    PROCESS_MEMORY_COUNTERS_EX pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc)))
        return pmc.PrivateUsage / 1024 / 1024 ; // MB
    else 
        return 0;
}

auto os::memory_peak() -> size_t
{
    PROCESS_MEMORY_COUNTERS_EX pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc)))
        return pmc.PeakWorkingSetSize / 1024 / 1024 ; // MB
    else 
        return 0;
}

auto os::get_proc_address(const char* module, const char* sym) -> void* {

    void* lib = nullptr;
    void* address = nullptr;
    std::string failreson;

    lib = [&module](){
        void* handle = GetModuleHandleA(module);
        return handle ? handle : LoadLibraryA(module);
    }();

    failreson = lib ? "" : std::to_string(GetLastError());
    address = (void *) ::GetProcAddress(reinterpret_cast<HMODULE>(lib), sym);

    if(lib == nullptr){
        throw Exception("Couldn't load lib {} reason: {}, fn name: {}", module, failreson, sym);
    }

    if(address == nullptr){
        throw Exception("Couldn't load symbole {} reason: {}", sym, failreson);
    }

    return address;
}