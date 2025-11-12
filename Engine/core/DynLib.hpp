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
    DynLib(const char* lib) : m_handle(nullptr), m_name(lib) {
    }
    
    DynLib() : m_handle(nullptr), m_name("") {
    }

    ~DynLib(){
        unload();
    }

    auto load(const char* lib) -> void
    {
        m_name = lib;

        if(m_handle) throw Exception("Can't load lib `{}` befor unloding prev lib", lib);
    
        #if defined(WINDOWS_PLT)
            m_handle = (void*) LoadLibraryA(full_name().c_str());
        #elif defined(LINUX_PLT) || defined(WEB_PLT)
            m_handle = (void*) dlopen(full_name().c_str(), RTLD_LAZY);
        #endif


        if (!m_handle) throw Exception("Can't open lib `{}`: {}", full_name().c_str(), error());
    }

    auto load() -> void
    {
        if(m_name.empty()) throw Exception("you should use load(string) instead .");
        load(m_name.c_str());
    }

    auto unload() -> void
    {
        if (!is_loaded()) return;

        #if defined(WINDOWS_PLT)
            FreeLibrary((HMODULE)m_handle);
        #elif defined(LINUX_PLT) || defined(WEB_PLT)
            dlclose(m_handle);
        #endif
        
        m_handle = nullptr;
    }

    auto reload(const char* lib) -> void
    {
        if(is_loaded()){
            unload();
            load(lib);
        }
    }

    auto reload() -> void
    {
        reload(this->m_name.c_str());
    }

    auto function(const char* name) -> void*
    {
        if (!is_loaded()) {
            throw Exception("Library not loaded, cannot get function `{}`", name);
        }

        void* f = nullptr;
        #if defined(WINDOWS_PLT)
            f = reinterpret_cast<void*>(GetProcAddress((HMODULE) m_handle, name));
        #elif defined(LINUX_PLT) || defined(WEB_PLT)
            (void)dlerror();
            f = reinterpret_cast<void*>(dlsym(m_handle, name));
        #endif

        if (f == nullptr) throw Exception("Can't load symbol `{}`: {}", name, error());
    
        return f;
    }

    template <class FuncPtr>
    auto function(const char* name) -> FuncPtr
    {
        return reinterpret_cast<FuncPtr>(function(name));
    }

    auto error() -> std::string {
        #if defined(WINDOWS_PLT)
            auto errorMessageID = GetLastError();
            if(errorMessageID == 0) {
                return "No error";
            }
    
            LPSTR messageBuffer = nullptr;
            size_t size = FormatMessageA(
                FORMAT_MESSAGE_ALLOCATE_BUFFER | 
                FORMAT_MESSAGE_FROM_SYSTEM | 
                FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL, 
                errorMessageID, 
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                (LPSTR)&messageBuffer, 
                0, 
                NULL
            );
            
            if (size == 0) {
                return "Failed to get error message";
            }
    
            std::string message(messageBuffer, size);
            
            // Clean up the message
            while (!message.empty() && 
                   (message.back() == '\r' || message.back() == '\n' || 
                    message.back() == ' ' || !std::isprint(static_cast<unsigned char>(message.back())))) {
                message.pop_back();
            }
            
            LocalFree(messageBuffer);
            
            return message.empty() ? "Unknown error" : message;
            
        #elif defined(LINUX_PLT) || defined(WEB_PLT)
            const char* error = dlerror();
            return error ? error : "No error";
        #endif
    }

    auto last_write_time() -> std::filesystem::file_time_type
    {
        return std::filesystem::last_write_time(m_name);
    }

    auto name() -> std::string {
        return m_name;
    }

    auto full_name() -> std::string {
        return PREFIX + m_name + SUFFIX;
    }

    auto is_loaded() const -> bool {
        return m_handle != nullptr;
    }

    static constexpr auto PREFIX  = DYN_LIB_PREFIX;
    static constexpr auto SUFFIX  = sys::Target == sys::Target::Windows ? ".dll" : ".so";

private:
    void* m_handle;
    std::string m_name;
};