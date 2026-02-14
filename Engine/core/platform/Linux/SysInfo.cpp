#include "SysInfo.hpp"

#include <fstream>
#include <string>
#include <sstream>
#include <unistd.h>
#include <limits.h>
#include <libgen.h>
#include <sys/utsname.h>
#include <cassert>
#include <dlfcn.h>
#include "Exception.hpp"

auto os::system() -> os::Target
{
    return os::Target::Linux;
}
    
auto os::system_name() -> std::string
{
    return "linux";
}

auto os::arch() -> os::Arch
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