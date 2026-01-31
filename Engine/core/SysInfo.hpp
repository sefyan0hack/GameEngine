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

    ENGINE_EXPORT auto build_info() -> const char*;

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