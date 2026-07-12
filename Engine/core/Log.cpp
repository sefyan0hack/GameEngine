#include "Log.hpp"
#include <cstdio>
#include <ostream>
#include <fstream>
#include <chrono>
#include <cstdlib>
#include <format>
#include <Log.inl>

[[maybe_unused]] constexpr auto LOG_FILE = "Engine.log";

namespace logg {

    auto default_logger(LogLvl l, std::string msg) -> void {
        if(l > lvl) return;
        auto level_to_string = [](LogLvl ll) {
            switch (ll) {
                case LogLvl::Info:  return "INFO";
                case LogLvl::Warn:  return "WARN";
                case LogLvl::Error: return "ERROR";
                case LogLvl::Trace: return "TRACE";
                case LogLvl::NoLog:  return "NONE";
            }
            return "unrechable";
        };

        std::string final_msg = std::format("[{}] [{:<5}] : {}", std::chrono::system_clock::now(), level_to_string(l), msg);

        plt_out(final_msg.c_str());
        std::ofstream out(LOG_FILE, std::ios::app); out << final_msg << '\n';
    };

    logger_handler_t* logger = default_logger;

    [[noreturn]] auto unimpl(std::source_location loc) noexcept -> void
    {
        error("this | {} | unimplemented -> {}:{}:{}", loc.function_name(), loc.file_name(), loc.line(), loc.column());
        std::abort();
    }

    [[noreturn]] auto unrechable(const char* msg, std::source_location loc) noexcept -> void
    {
        error("this branch unrechable in `{}` {} -> {}:{}:{}", loc.function_name(), msg, loc.file_name(), loc.line(), loc.column());
        std::abort();
    }
}