#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <format>
#include <ctime>
#include <iomanip>
#include <engine_export.h>

//"{:%Y-%m-%d %H:%M:%OS}"

namespace debug {
  inline auto current_time() -> std::string
  {
    std::time_t t = std::time(nullptr);
    std::tm* tm = std::localtime(&t);
    std::ostringstream oss;
    oss << std::put_time(tm, "%Y-%m-%d %H:%M:%S %Z");
    return oss.str();
  }

  template <class ...Ts>
  auto Log(
    [[maybe_unused]] const std::format_string<Ts...>& fmt,
    [[maybe_unused]] Ts&& ... ts) -> std::string
  {
    auto formatted_msg = std::format(fmt, std::forward<Ts>(ts)...);
    return std::format("{} : {}\n", current_time(), formatted_msg);
  }
  

  template <class... Ts>
  inline auto print([[maybe_unused]] const std::format_string<Ts...>& fmt, [[maybe_unused]] Ts&&... ts) -> void
  {
    #if defined(DEBUG) && !defined(NDEBUG)
      if(std::getenv("TESTING_ENABLED") == nullptr) {
        std::cout << Log(fmt, std::forward<Ts>(ts)...);
      }
    #else
      if(std::getenv("COUT_TO_FILE") != nullptr){
        std::ofstream out("Engine.log", std::ios::app);
        out << Log(fmt, std::forward<Ts>(ts)...);
      }
    #endif
  }

  template <size_t N>
  inline auto print(const char (&str)[N]) -> void {
    debug::print("{}", static_cast<const char*>(str));
  }

  template <class T>
  inline auto print(T&& x) -> void
  {
    debug::print("{}", std::forward<T>(x));
  }

  #define THIS_STR std::format("{}", *this)
  #define THIS_PRINT debug::print("{}", *this)
  #define THIS_ADDR (const void*)this

  // #define CTOR_LOG 
  // #define DTOR_LOG 
  #define CTOR_LOG debug::print("({:p})  {} {}", (const void*)this, ::type_name<std::remove_pointer_t<decltype(this)>>(), *this);
  #define DTOR_LOG debug::print("({:p}) ~{} {}", (const void*)this, ::type_name<std::remove_pointer_t<decltype(this)>>(), *this);

} // namespace debug