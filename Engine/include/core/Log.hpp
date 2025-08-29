#pragma once

#include <source_location>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <format>
#include <ctime>
#include <iomanip>
#include <exception>
#include <core/stacktrace.hpp>

//"{:%Y-%m-%d %H:%M:%OS}"

namespace Debug {
  inline auto CURR_TIME() -> std::string
  {
    std::time_t t = std::time(nullptr);
    std::tm* tm = std::localtime(&t);
    std::ostringstream oss;
    oss << std::put_time(tm, "%Y-%m-%d %H:%M:%S %Z");
    return oss.str();
  }

  template <typename ...Ts>
  auto Log(
    [[maybe_unused]] const std::format_string<Ts...>& fmt,
    [[maybe_unused]] Ts&& ... ts) -> std::string
  {
    auto formatted_msg = std::format(fmt, std::forward<Ts>(ts)...);
    return std::format("{} : {}\n", CURR_TIME(), formatted_msg);
  }
  

  template <typename... Ts>
  inline auto Print([[maybe_unused]] const std::format_string<Ts...>& fmt, [[maybe_unused]] Ts&&... ts) -> void
  {
    #if defined(DEBUG) && !defined(NDEBUG)
      if(std::getenv("TESTING_ENABLED") == nullptr) {
        std::cout << Log(fmt, std::forward<Ts>(ts)...);
      }
    #else
      if(std::getenv("COUT_TO_FILE") != nullptr){
        std::ofstream cout("Engine.log", std::ios::app);
        cout << Log(fmt, std::forward<Ts>(ts)...);
      }
    #endif
  }

  template <size_t N>
  inline auto Print(const char (&str)[N]) -> void {
    Print("{}", static_cast<const char*>(str));
  }

  template <typename T>
  inline auto Print(T&& x) -> void
  {
    Print("{}", std::forward<T>(x));
  }

} // namespace Debug


class CException final : public std::runtime_error {
public:

  template <typename... Ts>
  CException(
    const std::format_string<Ts...>& fmt,
    Ts&&... args
  )
    : std::runtime_error(std::format(fmt, std::forward<Ts>(args)...))
    , m_Trace(stacktrace::current(1))
  {}

  auto what() const noexcept  -> const char* override {
    return std::runtime_error::what();
  }

  auto trace() const noexcept -> std::string {
    return to_string(m_Trace);
  }

  auto where() const noexcept -> std::string {
    return !m_Trace.empty() ? m_Trace.begin()->description() : "??";
  }

  auto location() const noexcept -> std::string {
    return !m_Trace.empty() ? std::format("{}:{}", m_Trace.begin()->source_file(), m_Trace.begin()->source_line()) : "??:??";
  }


  auto all() const noexcept -> std::string
  {
    return std::format(
      "(Exception) at [{}] in {}\n"
      "\t-> what : `{}`\n"
      "{}",
      location(), where(), what(),
      trace()
    );
  }

private:
  stacktrace m_Trace;
};

#ifndef Try
#define Try(f) try {f;} catch(...){}
#endif

#ifndef Expect
#define Expect(cond, ...) if (!(cond)) throw CException("Expectation ["#cond"] Failed : " __VA_ARGS__);
#endif