#pragma once
#include <cstdint>
#include <engine_export.h>

namespace os
{

    ENGINE_EXPORT auto build_info() -> const char*;

    ENGINE_EXPORT auto name() -> const char*;
    ENGINE_EXPORT auto version() -> const char*;
    ENGINE_EXPORT auto arch() -> const char*;
    ENGINE_EXPORT auto bits() -> size_t;
    ENGINE_EXPORT auto proc_id() -> size_t;
    ENGINE_EXPORT auto memory_usage() -> size_t;
    ENGINE_EXPORT auto memory_peak() -> size_t;

    ENGINE_EXPORT auto get_proc_address(const char* module, const char* sym) -> void*; 
}