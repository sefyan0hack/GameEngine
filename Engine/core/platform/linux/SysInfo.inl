

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
        default:
            return "unknown";
    }
}