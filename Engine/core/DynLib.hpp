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
    DynLib(const char* lib) : m_handle(nullptr), m_file_path(PREFIX + std::string(lib) + SUFFIX), m_last_mod(mod_time()) {
    }
    
    DynLib() : m_handle(nullptr), m_file_path(""), m_last_mod(0) {
    }

    ~DynLib(){
        unload();
    }

    auto load(const char* lib) -> void
    {
        unload(); // Unload any existing library first
        
        m_file_path = PREFIX + std::string(lib) + SUFFIX;
        m_last_mod = mod_time();
    
        #if defined(WINDOWS_PLT)
            m_handle = (void*) LoadLibraryA(m_file_path.c_str());
        #elif defined(LINUX_PLT) || defined(WEB_PLT)
            m_handle = (void*) dlopen(m_file_path.c_str(), RTLD_LAZY);
        #endif

        if (m_handle == nullptr) throw Exception("Can't open lib `{}`: {}", m_file_path.c_str(), error());
    }

    auto load() -> void
    {
        if(m_file_path.empty()) throw Exception("you should use load(string) instead .");
        load(m_file_path.c_str());
    }

    auto unload() -> void
    {
        if (m_handle == nullptr) return;

        #if defined(WINDOWS_PLT)
            FreeLibrary((HMODULE)m_handle);
        #elif defined(LINUX_PLT) || defined(WEB_PLT)
            dlclose(m_handle);
        #endif
        
        m_handle = nullptr;
    }

    auto reload() -> bool
    {
        auto current_mod = mod_time();
        if (current_mod != m_last_mod) {
            unload();
            try {
                load();
                m_last_mod = current_mod;
                return true;
            } catch (...) {
                return false;
            }
        }
        return false;
    }

    template <class FuncPtr>
    auto function(const char* name) -> FuncPtr
    {
        if (m_handle == nullptr) {
            throw Exception("Library not loaded, cannot get function `{}`", name);
        }

        FuncPtr f = nullptr;
        #if defined(WINDOWS_PLT)
            f = reinterpret_cast<FuncPtr>(GetProcAddress((HMODULE)m_handle, name));
        #elif defined(LINUX_PLT) || defined(WEB_PLT)
            (void)dlerror();
            f = reinterpret_cast<FuncPtr>(dlsym(m_handle, name));
        #endif

        if (f == nullptr) throw Exception("Can't load symbol `{}`: {}", name, error());
    
        return f;
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
    
    static auto file_mod_time(const std::string& file) -> std::time_t 
    {
        try {
            auto ftime = std::filesystem::last_write_time(file);
            return std::chrono::system_clock::to_time_t(std::chrono::clock_cast<std::chrono::system_clock>(ftime));
        } catch (...) {
            return 0;
        }
    }

    auto mod_time() -> std::time_t 
    {
        return file_mod_time(m_file_path);
    }

    auto update_mod_time(std::time_t t) -> void
    {
        m_last_mod = t;
    }

    auto name() -> std::string {
        return m_file_path;
    }

    auto is_loaded() const -> bool {
        return m_handle != nullptr;
    }

    static constexpr auto PREFIX  = "lib";
    static constexpr auto SUFFIX = 
    #if defined(WINDOWS_PLT)
        ".dll";
    #elif defined(LINUX_PLT) || defined(WEB_PLT)
        ".so";
    #endif

private:
    void* m_handle;
    std::string m_file_path;
    std::time_t m_last_mod;
};