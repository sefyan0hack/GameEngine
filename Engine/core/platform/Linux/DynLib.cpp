#include "DynLib.hpp"
#include "Exception.hpp"
#include <string>

#include <dlfcn.h>

auto DynLib::load() -> void
{
    auto f_name = full_name();
    if(m_handle) throw Exception("Can't load lib `{}` before unloding prev lib", f_name);

    if(m_name.empty()) m_handle = (void*) dlopen(nullptr, RTLD_NOW | RTLD_LOCAL);
    else m_handle = (void*) dlopen(f_name.c_str(), RTLD_NOW | RTLD_LOCAL);

    if (!m_handle) throw Exception("Can't open lib `{}`: {}", f_name, error());
}

auto DynLib::unload() -> void
{
    if (!is_loaded()) return;

    bool r = true;
    if(dlclose(m_handle) != 0) r = false;
    if(r == false) throw Exception("Cant unload lib `{}` : ", m_name, error()); 
    m_handle = nullptr;
}

auto DynLib::symbole(const char* name) -> void*
{
    if (!is_loaded()) throw Exception("Library not loaded, cannot get symbole `{}`", name);    
    auto f = reinterpret_cast<void*>(dlsym(m_handle, name));
    if (f == nullptr) throw Exception("Can't load symbol `{}`: {}", name, error());
    return f;
}

auto DynLib::error() -> std::string {
    const char* error = dlerror();
    return error ? error : "No error";
}