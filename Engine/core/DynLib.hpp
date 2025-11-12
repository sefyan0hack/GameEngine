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

    DynLib(const DynLib& other)
        : m_handle(other.m_handle)
        , m_name(other.m_name)
    {}

    DynLib(DynLib&& other)
        : m_handle(std::move(other.m_handle))
        , m_name(std::move(other.m_name))
    {}

    auto operator=(const DynLib& other) -> DynLib&
    {
        if(this != &other){
            this->m_handle = other.m_handle;
            this->m_name = other.m_name;
        }
        return *this;
    }
    auto operator=(DynLib&& other) -> DynLib&
    {
        if(this != &other){
            this->m_handle = std::move(other.m_handle);
            this->m_name = std::move(other.m_name);

            other.m_handle = nullptr;
            other.m_name = "" ;
        }
        return *this;
    }

    ~DynLib(){
        unload();
    }

    auto load() -> void
    {

        if(m_handle) throw Exception("Can't load lib `{}` befor unloding prev lib", m_name);

        #if defined(WINDOWS_PLT)
            m_handle = (void*) LoadLibraryA(full_name().c_str());
        #elif defined(LINUX_PLT) || defined(WEB_PLT)
            m_handle = (void*) dlopen(full_name().c_str(), RTLD_NOW | RTLD_LOCAL);
        #endif

        if (!m_handle) throw Exception("Can't open lib `{}`: {}", full_name().c_str(), error());
    }

    auto unload() -> void
    {
        if (!is_loaded()) return;
        bool r = true;
        #if defined(WINDOWS_PLT)
            if(FreeLibrary((HMODULE)m_handle) == 0) r = false;
        #elif defined(LINUX_PLT) || defined(WEB_PLT)
            if(dlclose(m_handle) != 0) r = false;
        #endif

        if(r == false)
            throw Exception("Cant unload lib `{}` : ", m_name, error());

        m_handle = nullptr;
    }

    auto reload() -> void
    {
        if(is_loaded()){
            unload();
            load();
        }
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