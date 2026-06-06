#include "../Unix/SysInfo.cpp"

auto os::memory_usage() -> std::size_t
{
    return get_proc_value("VmRSS:") / 1024;
}

auto os::memory_peak() -> std::size_t
{
    return get_proc_value("VmHWM:") / 1024;
}
