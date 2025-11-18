// platform/windows/Sysinfo.inl

auto sys::host::name() -> sys::Target
{
    return sys::Target::Windows;
}

auto sys::host::name_str() -> std::string
{
    return "windows";
}

auto sys::host::arch() -> sys::Arch
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

auto sys::host::arch_str() -> std::string
{
    switch (arch())
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