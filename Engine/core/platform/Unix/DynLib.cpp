#include "DynLib.hpp"
#include "Exception.hpp"
#include <string>

#include <dlfcn.h>

auto DynLib::load() -> void
{
    if(m_handle) throw Exception("Can't load lib `{}` before unloding prev lib", m_name);

    m_handle = (void*) dlopen(full_name().c_str(), RTLD_NOW | RTLD_LOCAL);
    
    if (!m_handle) throw Exception("Can't open lib `{}`: {}", full_name().c_str(), error());
}

auto DynLib::unload() -> void
{
    if (!is_loaded()) return;

    bool r = true;
    if(dlclose(m_handle) != 0) r = false;
    if(r == false) throw Exception("Cant unload lib `{}` : ", m_name, error()); 
    m_handle = nullptr;
}

auto DynLib::function(const char* name) -> void*
{
    if (!is_loaded()) throw Exception("Library not loaded, cannot get function `{}`", name);    
    auto f = reinterpret_cast<void*>(dlsym(m_handle, name));
    if (f == nullptr) throw Exception("Can't load symbol `{}`: {}", name, error());
    return f;
}

auto DynLib::error() -> std::string {
    const char* error = dlerror();
    return error ? error : "No error";
}