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
    return "Linux";
}

auto os::arch() -> os::Arch
{
    utsname system_info;
    uname(&system_info);

    std::string machine = system_info.machine;
    
    if (machine == "x86_64" || machine == "amd64" || machine == "i386" || machine == "i686" || machine == "x86") {
        return Arch::x86_x64;
    } else if (machine.find("arm") != std::string::npos) {
        return Arch::arm;
    } else {
        return Arch::unknown;
    }
}