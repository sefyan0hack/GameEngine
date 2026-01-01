#include "Log.hpp"
#include <iostream>
#include <fstream>
#include <chrono>
#include <exception>

#if defined(ANDROID_PLT)
#include <android/log.h>
#endif

namespace debug {
    auto log(const char* msg) -> void
    {
        auto new_msg = std::format("{} : {}\n", std::chrono::system_clock::now(), msg);

        #if defined(ANDROID_PLT)
        #define LOG_LOGIC __android_log_write(ANDROID_LOG_INFO, "Engine", new_msg.c_str())
        #else
        #define LOG_LOGIC  std::cout << new_msg
        #endif

        # if defined(DEBUG) && !defined(NDEBUG)
        if(std::getenv("TESTING_ENABLED") == nullptr) {
            LOG_LOGIC;
        }
        # else
        if(std::getenv("COUT_TO_FILE") != nullptr){
            std::ofstream out("Engine.log", std::ios::app);
            out << new_msg;
        }
        # endif
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