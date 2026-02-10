#include "DynLib.hpp"
#include "Exception.hpp"
#include <string>

DynLib::DynLib(const char* lib, bool now) : m_handle(nullptr), m_name(lib) { if(now) load(); }
DynLib::~DynLib(){ unload(); }

auto DynLib::name() -> std::string { return m_name; }
auto DynLib::full_name() -> std::string { return DYN_LIB_PREFIX + m_name + DYN_LIB_SUFFIX; }
auto DynLib::is_loaded() const -> bool { return m_handle != nullptr; }

auto DynLib::reload() -> void
{
    if(is_loaded()){
        unload();
        load();
    }
}