#pragma once
#include <string>
#include <engine_export.h>

namespace sys
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
        constexpr auto name() -> sys::Target;
        constexpr auto name_str() -> std::string;
        constexpr auto arch() -> sys::Arch;
        constexpr auto arch_str() -> std::string;
    }

    namespace host
    {
        ENGINE_EXPORT auto name() -> sys::Target;
        ENGINE_EXPORT auto name_str() -> std::string;
        ENGINE_EXPORT auto arch() -> sys::Arch;
        ENGINE_EXPORT auto arch_str() -> std::string;
    }
}