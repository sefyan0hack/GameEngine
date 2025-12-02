// platform/windows/Sysinfo.inl

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