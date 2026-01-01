#include "Log.hpp"
#include <print>
#include <ostream>
#include <fstream>
#include <chrono>
#include <exception>


constexpr auto LOG_FILE = "Engine.log";

namespace debug {
    auto log(const char* msg) -> void
    {
        #if !defined(NO_CONSOLE)
            std::println("{} : {}", std::chrono::system_clock::now(), msg);
        #else
            std::ofstream out(LOG_FILE, std::ios::app);
            std::println(out, "{} : {}", std::chrono::system_clock::now(), msg);
        #endif
    }

    auto log(std::string msg) -> void
    {
        return log(msg.c_str());
    }
    
    [[noreturn]] auto unimpl(std::source_location loc) noexcept -> void
    {
        log("unimplemented -> {}:{}", loc.file_name(), loc.line());
        std::terminate();
    }
}