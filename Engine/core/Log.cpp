#include "Log.hpp"
#include <print>
#include <ostream>
#include <fstream>
#include <chrono>
#include <cstdlib>

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
        log("this | {} | unimplemented -> {}:{}:{}", loc.function_name(), loc.file_name(), loc.line(), loc.column());
        std::abort();
    }

    [[noreturn]] auto unrechable(const char* msg, std::source_location loc) noexcept -> void
    {
        log("this branch unrechable in `{}` {} -> {}:{}:{}", loc.function_name(), msg, loc.file_name(), loc.line(), loc.column());
        std::abort();
    }
}