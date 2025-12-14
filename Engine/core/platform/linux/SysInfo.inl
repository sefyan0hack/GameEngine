
#include <fstream>
#include <string>
#include <sstream>
#include <unistd.h>
#include <limits.h>
#include <libgen.h>


auto os::host::name_tag() -> os::Target
{
    return os::Target::Linux;
}
    
auto os::host::name() -> std::string
{
    return "linux";
}

auto os::host::arch_tag() -> os::Arch
{
    utsname system_info;
    uname(&system_info);

    std::string machine = system_info.machine;
    
    if (machine == "x86_64" || machine == "amd64") {
        return Arch::x64;
    } else if (machine == "i386" || machine == "i686" || machine == "x86") {
        return Arch::x86;
    } else if (machine.find("arm") != std::string::npos) {
        if (machine.find("arm64") != std::string::npos || 
            machine.find("aarch64") != std::string::npos) {
            return Arch::arm64;
        } else {
            return Arch::arm;
        }
    } else if (machine == "aarch64") {
        return Arch::arm64;
    } else {
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
    // getpid() returns a pid_t (typically an int or long), which we cast to std::size_t.
    return static_cast<std::size_t>(getpid());
}

// Helper to parse /proc/self/status for specific keys
static std::size_t get_proc_value(const std::string& key)
{
    std::ifstream statusFile("/proc/self/status");
    std::string line;
    while (std::getline(statusFile, line))
    {
        if (line.compare(0, key.length(), key) == 0)
        {
            std::istringstream iss(line);
            std::string k;
            std::size_t value;
            std::string unit; // "kB"
            iss >> k >> value >> unit;
            return value;
        }
    }
    return 0;
}

auto os::host::memory_usage() -> std::size_t
{
    // VmRSS (Resident Set Size) is the closest match to "Private Usage" 
    std::size_t rss_kb = get_proc_value("VmRSS:");
    return rss_kb / 1024; //  kB -> MB
}

auto os::host::memory_peak() -> std::size_t
{
    // VmHWM (High Water Mark) is the peak resident set size.
    std::size_t hwm_kb = get_proc_value("VmHWM:");
    return hwm_kb / 1024; // kB -> MB
}

auto os::host::thread_count() -> std::size_t
{
    return get_proc_value("Threads:");
}

auto os::host::module() -> std::string
{
    char path[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", path, sizeof(path) - 1);

    if (len != -1)
    {
        path[len] = '\0';
        return std::string(basename(path));
    }
    else
    {
        return "";
    }
}