#include "DynLib.hpp"
#include "Exception.hpp"
#include <string>

#include <windows.h>

auto DynLib::load() -> void
{ 
    if(m_handle) throw Exception("Can't load lib `{}` before unloding prev lib", m_name);
    m_handle = (void*) LoadLibraryA(full_name().c_str());
    if (!m_handle) throw Exception("Can't open lib `{}`: {}", full_name().c_str(), error());
}

auto DynLib::unload() -> void
{
    if (!is_loaded()) return;

    bool r = true;
    if(FreeLibrary((HMODULE)m_handle) == 0) r = false;

    if(r == false)
        throw Exception("Cant unload lib `{}` : ", m_name, error());

    m_handle = nullptr;
}

auto DynLib::function(const char* name) -> void*
{
    if (!is_loaded()) throw Exception("Library not loaded, cannot get function `{}`", name);
    auto f = reinterpret_cast<void*>(GetProcAddress((HMODULE) m_handle, name));
    if (f == nullptr) throw Exception("Can't load symbol `{}`: {}", name, error());
    return f;
}

auto DynLib::error() -> std::string {

    auto errorMessageID = GetLastError();

    if(errorMessageID == 0) return "No error";

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
}
