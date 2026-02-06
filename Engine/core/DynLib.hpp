#pragma once

#include <string>
#include <engine_export.h>


class ENGINE_EXPORT DynLib {
public:
    DynLib(const char* lib, bool now = true);
    DynLib(const DynLib& other);
    DynLib(DynLib&& other);

    auto operator=(const DynLib& other) -> DynLib&;
    auto operator=(DynLib&& other) -> DynLib&;
    
    ~DynLib();

    auto load() -> void;
    auto unload() -> void;
    auto reload() -> void;
    auto function(const char* name) -> void*;

    template <class FuncPtr>
    auto function(const char* name) -> FuncPtr
    {
        return reinterpret_cast<FuncPtr>(function(name));
    }

    auto error() -> std::string;
    auto name() -> std::string;
    auto full_name() -> std::string;
    auto is_loaded() const -> bool;

private:
    void* m_handle;
    std::string m_name;
};