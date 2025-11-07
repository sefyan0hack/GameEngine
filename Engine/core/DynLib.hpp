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
    DynLib(const char* lib) : m_handle(nullptr), m_file_path(std::string(lib) + EXT), m_last_mod(mod_time()) {
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

        if (m_handle == nullptr) throw Exception("Can't open lib `{}`: {}", m_file_path, error());
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


    template <class FuncPtr>
    auto get_function(const char* name) -> FuncPtr
    {
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
    
            // Create std::string and clean up any trailing whitespace or non-printable chars
            std::string message(messageBuffer, size);
            
            // Clean up the message - remove trailing CR/LF and other non-printable characters
            while (!message.empty() && 
                   (message.back() == '\r' || message.back() == '\n' || 
                    message.back() == ' ' || !std::isprint(static_cast<unsigned char>(message.back())))) {
                message.pop_back();
            }
            
            // Alternative: use a more robust cleaning approach
            std::string clean_message;
            for (char c : message) {
                if (std::isprint(static_cast<unsigned char>(c)) || c == ' ') {
                    clean_message += c;
                } else if (c == '\r' || c == '\n' || c == '\t') {
                    clean_message += ' '; // Replace control chars with space
                }
                // Skip other non-printable characters
            }
            
            // Trim leading/trailing spaces from clean_message
            size_t start = clean_message.find_first_not_of(" \t\r\n");
            size_t end = clean_message.find_last_not_of(" \t\r\n");
            
            if (start != std::string::npos && end != std::string::npos) {
                clean_message = clean_message.substr(start, end - start + 1);
            } else {
                clean_message.clear();
            }
            
            LocalFree(messageBuffer);
            
            return clean_message.empty() ? "Unknown error (non-printable characters)" : clean_message;
            
        #elif defined(LINUX_PLT) || defined(WEB_PLT)
            const char* error = dlerror();
            return error ? error : "No error";
        #endif
    }
    
    static auto file_mod_time(const std::string& file) -> std::time_t 
    {
        auto ftime = std::filesystem::last_write_time(file);
        return std::chrono::system_clock::to_time_t( std::chrono::clock_cast<std::chrono::system_clock>(ftime) );
    }

    auto mod_time() -> std::time_t 
    {
        return file_mod_time(m_file_path);
    }

    auto upadte_mod_time(std::time_t t) -> void
    {
        m_last_mod = t;
    }

    static constexpr auto EXT = 
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