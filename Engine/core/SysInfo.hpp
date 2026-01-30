#pragma once
#include <string>
#include <engine_export.h>
#include "format.hpp"

namespace os
{
     /// @brief enum of Target systems
    enum class Target : uint8_t
    {
        Windows,
        Linux,
        Web,
        Android
    };

    /// @brief enum of Arch systems
    enum class Arch : uint8_t
    {
        x64,
        x86,
        arm,
        arm64,
        wasm,
        wasm64,
        unknown
    };

    constexpr auto build_system_name = SYSTEM_HOST_NAME;
    constexpr auto build_system_version = SYSTEM_HOST_VERSION;
    constexpr auto build_system_arch = SYSTEM_HOST_ARCH;

    constexpr auto build_compiler_name = COMPILER_NAME;
    constexpr auto build_compiler_version = COMPILER_VERSION;

    constexpr auto build_linker_name = LINKER_NAME;
    constexpr auto build_linker_version = LINKER_VERSION;

    constexpr auto build_cxx_std = CXX_STANDARD;

    constexpr auto build_timestamp = __TIMESTAMP__;

    ENGINE_EXPORT auto build_info() -> std::string;

    ENGINE_EXPORT auto system() -> os::Target;
    ENGINE_EXPORT auto system_name() -> std::string;
    ENGINE_EXPORT auto arch() -> os::Arch;
    ENGINE_EXPORT auto arch_name() -> std::string;
    ENGINE_EXPORT auto proc_id() -> std::size_t;
    ENGINE_EXPORT auto memory_usage() -> std::size_t;
    ENGINE_EXPORT auto memory_peak() -> std::size_t;
    ENGINE_EXPORT auto thread_count() -> std::size_t;
    
    ENGINE_EXPORT auto get_proc_address(const char* module, const char* sym) -> void*; 
}