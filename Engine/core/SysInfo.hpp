#pragma once
#include <cstddef>
#include <engine_export.h>

namespace os
{

    ENGINE_EXPORT auto build_info() -> const char*;

    ENGINE_EXPORT auto name() -> const char*;
    ENGINE_EXPORT auto arch() -> const char*;
    ENGINE_EXPORT auto bits() -> std::size_t;
    ENGINE_EXPORT auto proc_id() -> std::size_t;
    ENGINE_EXPORT auto memory_usage() -> std::size_t;
    ENGINE_EXPORT auto memory_peak() -> std::size_t;

    ENGINE_EXPORT auto get_proc_address(const char* module, const char* sym) -> void*; 
}