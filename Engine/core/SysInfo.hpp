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
        constexpr auto name_tag() -> sys::Target;
        constexpr auto name() -> std::string;
        constexpr auto arch_tag() -> sys::Arch;
        constexpr auto arch() -> std::string;
    }

    namespace host
    {
        ENGINE_EXPORT auto name_tag() -> sys::Target;
        ENGINE_EXPORT auto name() -> std::string;
        ENGINE_EXPORT auto arch_tag() -> sys::Arch;
        ENGINE_EXPORT auto arch() -> std::string;
    }
}