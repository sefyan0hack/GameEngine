#include "SysInfo.hpp"
#include "format.hpp"

auto os::build_info() -> std::string
{
    return ::format(
        "Build info: \n",
        " - system_name: {}\n",
        " - system_version: {}\n",
        " - system_arch: {}\n",
        " - compiler_name: {}\n",
        " - compiler_version: {}\n",
        " - linker_name: {}\n",
        " - linker_version: {}\n",
        " - cxx_std: {}\n",
        " - timestamp: {}\n",
        build_system_name, build_system_version, build_system_arch,
        build_compiler_name, build_compiler_version,
        build_linker_name, build_linker_version,
        build_cxx_std,
        build_timestamp
    );
}