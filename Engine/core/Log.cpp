#include "Log.hpp"
#include <cstdio>
#include <ostream>
#include <fstream>
#include <chrono>
#include <cstdlib>

#if defined(ANDROID_PLT)
#include <android/log.h>
#endif

constexpr auto LOG_FILE = "Engine.log";

namespace debug {
    auto default_logger(std::string msg ) -> void {
        auto now = std::format("{}", std::chrono::system_clock::now());

        #if defined(CONSOLE_ATTACHED)
        #   if defined(ANDROID_PLT)
            __android_log_printf(ANDROID_LOG_INFO, "ENGINE", "%s : %s", now.c_str(), msg.c_str());
        #   else
            std::printf("%s : %s\n", now.c_str(), msg.c_str());
        #   endif
        #else
            std::ofstream out(LOG_FILE, std::ios::app);
            out << now << " : " << msg;
        #endif
    };

    logger_handler_t* logger = default_logger;

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