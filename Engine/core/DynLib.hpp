#pragma once

#include <string>
#include <engine_export.h>

class ENGINE_EXPORT DynLib {
public:
    DynLib(std::string name, bool now = true);

    DynLib(const DynLib& other) = delete;
    auto operator=(const DynLib& other) -> DynLib& = delete;

    DynLib(DynLib&& other) = delete;
    auto operator=(DynLib&& other) -> DynLib& = delete;

    ~DynLib();

    auto load() -> void;
    auto unload() -> void;
    auto reload() -> void;
    auto symbole(const char* name) -> void*;

    template <class T>
    auto symbole(const char* name) -> T
    {
        return reinterpret_cast<T>(symbole(name));
    }

    auto name() -> std::string;
    auto error() -> std::string;
    auto full_name() -> std::string;
    auto is_loaded() const -> bool;

private:
    void* m_handle;
    std::string m_name;
};