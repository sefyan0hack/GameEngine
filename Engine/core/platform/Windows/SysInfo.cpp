#include "SysInfo.hpp"
#include "Log.hpp"
#include <windows.h>
#include <psapi.h>
#include <tlhelp32.h>
#include "Exception.hpp"
#include <string>

auto os::system() -> os::Target
{
    return os::Target::Windows;
}

auto os::system_name() -> std::string
{
    return "windows";
}

auto os::arch() -> os::Arch
{
    SYSTEM_INFO SYS_INFO{};
    GetSystemInfo(&SYS_INFO);

    switch (SYS_INFO.wProcessorArchitecture)
    {
        case PROCESSOR_ARCHITECTURE_AMD64:
            return Arch::x64;
        case PROCESSOR_ARCHITECTURE_INTEL:
            return Arch::x86;
        case PROCESSOR_ARCHITECTURE_ARM:
            return Arch::arm;
        case PROCESSOR_ARCHITECTURE_ARM64:
            return Arch::arm64;
        default:
            return Arch::unknown;
    }
}

auto os::proc_id() -> std::size_t
{
    return static_cast<std::size_t>(GetCurrentProcessId());
}

auto os::memory_usage() -> std::size_t
{
    PROCESS_MEMORY_COUNTERS_EX pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc)))
        return pmc.PrivateUsage / 1024 / 1024 ; // MB
    else 
        return 0;
}

auto os::memory_peak() -> std::size_t
{
    PROCESS_MEMORY_COUNTERS_EX pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc)))
        return pmc.PeakWorkingSetSize / 1024 / 1024 ; // MB
    else 
        return 0;
}

auto os::thread_count() -> std::size_t
{    
    // Take a snapshot of all processes in the system
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE)
        return 0;

    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    // Retrieve information about the first process
    if (!Process32First(hSnapshot, &pe32))
    {
        CloseHandle(hSnapshot);
        return 0;
    }

    std::size_t count = 0;

    // Iterate through the snapshot to find our process
    do
    {
        if (pe32.th32ProcessID == proc_id())
        {
            count = pe32.cntThreads;
            break; // Found our process, no need to continue
        }
    } while (Process32Next(hSnapshot, &pe32));

    CloseHandle(hSnapshot);
    return count;
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