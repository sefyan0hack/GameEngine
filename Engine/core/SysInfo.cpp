#include "SysInfo.hpp"
#include "format.hpp"

auto os::build_info() -> const char*
{
    return "Build info: \n"
    " - system_name: " SYSTEM_HOST_NAME "\n"
    " - system_version: " SYSTEM_HOST_VERSION "\n"
    " - system_arch: " SYSTEM_HOST_ARCH "\n"
    " - compiler_name: " COMPILER_NAME "\n"
    " - compiler_version: " COMPILER_VERSION "\n"
    " - cxx_std: " CXX_STANDARD "\n"
    " - timestamp: " __TIMESTAMP__ "\n";  
}