#pragma once
#include <string>
#include <engine_export.h>

namespace os
{
     /// @brief enum of Target systems
    enum class Target : uint8_t
    {
        Windows,
        Linux,
        Web
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

    namespace build
    {
        constexpr const char* TimeStamp = __TIMESTAMP__;
        consteval auto name_tag() -> os::Target;
        consteval auto name() -> std::string;
        consteval auto arch_tag() -> os::Arch;
        consteval auto arch() -> std::string;
    }

    namespace host
    {
        ENGINE_EXPORT auto name_tag() -> os::Target;
        ENGINE_EXPORT auto name() -> std::string;
        ENGINE_EXPORT auto arch_tag() -> os::Arch;
        ENGINE_EXPORT auto arch() -> std::string;
        ENGINE_EXPORT auto proc_id() -> std::size_t;
        ENGINE_EXPORT auto memory_usage() -> std::size_t;
        ENGINE_EXPORT auto memory_peak() -> std::size_t;
        ENGINE_EXPORT auto thread_count() -> std::size_t; 
        ENGINE_EXPORT auto module() -> std::string;
    }
}