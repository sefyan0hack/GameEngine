#pragma once

#include "Log.hpp"
#include <ctime>
#include <chrono>
#include <filesystem>

#if defined(WINDOWS_PLT)
    #include <windows.h>
#elif defined(LINUX_PLT) || defined(WEB_PLT)
    #include <dlfcn.h>
#endif

class DynLib {
public:
    DynLib(const char* lib) : m_handle(nullptr), m_file_path(lib), m_last_mod(mod_time()) {
    }

    ~DynLib(){
    }

    auto load() -> void
    {
        #if defined(WINDOWS_PLT)
            m_handle = (void*) LoadLibraryA(m_file_path.c_str());
        #elif defined(LINUX_PLT) || defined(WEB_PLT)
            m_handle = (void*) dlopen(m_file_path.c_str(), RTLD_LAZY);
        #endif

        if (!m_handle) Exception("Can't open lib `{}`: {}", m_file_path, error());
    }

    auto unload() -> void
    {
        #if defined(WINDOWS_PLT)
            FreeLibrary((HMODULE)m_handle);
        #elif defined(LINUX_PLT) || defined(WEB_PLT)
            dlclose(m_handle);
        #endif
    }

    auto reload() -> void
    {
        unload();
        load();
    }


    template <typename R, typename... Args>
    auto get_function(const char* name) -> R(*)(Args...)
    {
        #if defined(WINDOWS_PLT)
            return (R(*)(Args...)) GetProcAddress((HMODULE)m_handle, name);
        #elif defined(LINUX_PLT) || defined(WEB_PLT)
            return (R(*)(Args...)) dlsym(m_handle, name);
        #endif
    }

    auto error() -> std::string {
        #if defined(WINDOWS_PLT)
            const char* error = "GetLastError()";
        #elif defined(LINUX_PLT) || defined(WEB_PLT)
            const char* error = dlerror();
        #endif

        return error ? error : "No error";
    }
    
    static auto file_mod_time(const std::string& file) -> std::time_t 
    {
        auto ftime = std::filesystem::last_write_time(file);
        return std::chrono::system_clock::to_time_t( std::chrono::file_clock::to_sys(ftime) );
    }

    auto mod_time() -> std::time_t 
    {
        return file_mod_time(m_file_path);
    }

    auto upadte_mod_time(std::time_t t) -> void
    {
        m_last_mod = t;
    }

private:
    void* m_handle;
    std::string m_file_path;
    std::time_t m_last_mod;
};