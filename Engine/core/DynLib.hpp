#pragma once

#include "Log.hpp"

#if defined(WINDOWS_PLT)
    #include <windows.h>
#elif defined(LINUX_PLT) || defined(WEB_PLT)
    #include <dlfcn.h>
#endif

class DynLib {
public:
    DynLib(const char* lib){
        #if defined(WINDOWS_PLT)
            m_handle = (void*) LoadLibraryA(lib);
        #elif defined(LINUX_PLT) || defined(WEB_PLT)
            m_handle = (void*) dlopen(lib, RTLD_LAZY);
        #endif

        if (!m_handle) Exception("Can't open lib `{}`: {}", lib, error());
    }

    ~DynLib(){
        #if defined(WINDOWS_PLT)
            FreeLibrary((HMODULE)m_handle);
        #elif defined(LINUX_PLT) || defined(WEB_PLT)
            dlclose(m_handle);
        #endif
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
private:
    void* m_handle;
};