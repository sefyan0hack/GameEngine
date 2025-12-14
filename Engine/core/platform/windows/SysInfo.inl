// platform/windows/Sysinfo.inl
#include <psapi.h>
#include <tlhelp32.h>
#include "Log.hpp"

auto os::host::name_tag() -> os::Target
{
    return os::Target::Windows;
}

auto os::host::name() -> std::string
{
    return "windows";
}

auto os::host::arch_tag() -> os::Arch
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

auto os::host::arch() -> std::string
{
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
        case Arch::wasm64:
        case Arch::unknown:
        default:
            return "unknown";
    }
}

auto os::host::proc_id() -> std::size_t
{
    return static_cast<std::size_t>(GetCurrentProcessId());
}

auto os::host::memory_usage() -> std::size_t
{
    PROCESS_MEMORY_COUNTERS_EX pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc)))
        return pmc.PrivateUsage / 1024 / 1024 ; // MB
    else 
        return 0;
}

auto os::host::memory_peak() -> std::size_t
{
    PROCESS_MEMORY_COUNTERS_EX pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc)))
        return pmc.PeakWorkingSetSize / 1024 / 1024 ; // MB
    else 
        return 0;
}

auto os::host::thread_count() -> std::size_t
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

auto os::host::module() -> std::string
{
    auto proc = GetCurrentProcess();
    auto module = GetModuleHandle(nullptr);
    TCHAR szName[_MAX_FNAME];
    auto _ = GetModuleBaseName(proc, module, szName, _MAX_FNAME );
    return std::string(szName);
}