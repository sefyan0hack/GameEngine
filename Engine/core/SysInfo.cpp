#include "SysInfo.hpp"

auto os::build_info() -> const char*
{
    return "Build info: \n"
    " - host os name: " EG_SYSTEM_HOST_NAME "\n"
    " - host os version: " EG_SYSTEM_HOST_VERSION "\n"
    " - host os arch: " EG_SYSTEM_HOST_ARCH "\n"
    " - compiler name: " EG_COMPILER_NAME "\n"
    " - compiler version: " EG_COMPILER_VERSION "\n"
    " - build config: " EG_BUILD_CONFIG "\n"
    " - std: c++" EG_CXX_STANDARD "\n"
    " - timestamp: " __TIMESTAMP__ "\n";  
}

auto os::name() -> const char*
{
    return EG_SYSTEM_NAME;
}

auto os::arch() -> const char*
{
    return EG_SYSTEM_ARCH;
}

auto os::bits() -> size_t
{
    return sizeof(void*) * 8;
}
