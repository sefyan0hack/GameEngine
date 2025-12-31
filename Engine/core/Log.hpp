#pragma once

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <format>
#include <chrono>
#include <source_location>
#include <engine_export.h>

#if defined(ANDROID_PLT)
#include <android/log.h>
#endif

namespace debug {
  template <class ...Ts>
  auto Log(
    [[maybe_unused]] const std::format_string<Ts...>& fmt,
    [[maybe_unused]] Ts&& ... ts) -> std::string
  {
    auto formatted_msg = std::format(fmt, std::forward<Ts>(ts)...);
    return std::format("{} : {}\n", std::chrono::system_clock::now(), formatted_msg);
  }


  template <class... Ts>
  inline auto log([[maybe_unused]] const std::format_string<Ts...>& fmt, [[maybe_unused]] Ts&&... ts) -> void
  {
    #if defined(ANDROID_PLT)
      #define LOG_LOGIC __android_log_write(ANDROID_LOG_INFO, "Engine", Log(fmt, std::forward<Ts>(ts)...).c_str())
    #else
      #define LOG_LOGIC  std::cout << Log(fmt, std::forward<Ts>(ts)...)
    #endif

    # if defined(DEBUG) && !defined(NDEBUG)
      if(std::getenv("TESTING_ENABLED") == nullptr) {
        LOG_LOGIC;
      }
    # else
      if(std::getenv("COUT_TO_FILE") != nullptr){
        std::ofstream out("Engine.log", std::ios::app);
        out << Log(fmt, std::forward<Ts>(ts)...);
      }
    # endif
  }

  template <size_t N>
  inline auto log(const char (&str)[N]) -> void {
    debug::log("{}", static_cast<const char*>(str));
  }

  template <class T>
  inline auto log(T&& x) -> void
  {
    debug::log("{}", std::forward<T>(x));
  }

  #define THIS_STR std::format("{}", *this)
  #define THIS_LOG debug::log("{}", *this)
  #define THIS_ADDR (const void*)this

  // #define CTOR_LOG 
  // #define DTOR_LOG 
  #define CTOR_LOG debug::log("({:p})  {} {}", (const void*)this, ::type_name<std::remove_pointer_t<decltype(this)>>(), *this);
  #define DTOR_LOG debug::log("({:p}) ~{}", (const void*)this, ::type_name<std::remove_pointer_t<decltype(this)>>());

  [[noreturn]] inline auto unimpl(std::source_location loc = std::source_location::current()) noexcept 
  {
    log("unimplimented -> {}:{}", loc.file_name(), loc.line());
    std::terminate();
  }
} // namespace debug