#pragma once

#include <source_location>
#include <iostream>
#include <cstdlib>
#include <format>
#include <chrono>
#include <exception>

#include <core/stacktrace.hpp>

namespace Debug {

  inline std::string formatedTime() {
    const auto now = std::chrono::system_clock::now();
    return std::format("{:%Y-%m-%d %H:%M:%OS}", now);
  }

  template <typename ...Ts>
  auto Log(
    [[maybe_unused]] const std::format_string<Ts...>& fmt,
    [[maybe_unused]] Ts&& ... ts) -> void
  {
    auto formatted_msg = std::format(fmt, std::forward<Ts>(ts)...);

    if(std::getenv("TESTING_ENABLED") == nullptr){
      std::cout << std::format("{} : {}\n", formatedTime(), formatted_msg);
    }
  }
  

  template <typename... Ts>
  inline auto Print([[maybe_unused]] const std::format_string<Ts...>& fmt, [[maybe_unused]] Ts&&... ts) -> void
  {
    #if defined(DEBUG) && !defined(NDEBUG)
    Log(fmt, std::forward<Ts>(ts)...);
    #endif
  }

  template <typename T>
  inline auto Print(T&& x) -> void
  {
    #if defined(DEBUG) && !defined(NDEBUG)
    Log("{}", std::forward<T>(x));
    #endif
  }

} // namespace Debug


class CException final : public std::runtime_error {
public:
  using clock = std::chrono::system_clock;

  template <typename... Ts>
  CException(
    const std::format_string<Ts...>& fmt,
    Ts&&... args
  )
    : std::runtime_error(std::format(fmt, std::forward<Ts>(args)...))
    , m_Trace(stacktrace::current(1))
    , m_Timestamp(clock::now())
  {}

  auto what() const noexcept  -> const char* override {
    return std::runtime_error::what();
  }

  auto trace() const noexcept -> std::string {
    return to_string(m_Trace);
  }

  auto when() const noexcept -> std::string {
    return std::format("{:%Y-%m-%d %H:%M:%OS}", m_Timestamp);
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
      "{} : \n"
      "(Exception) at [{}] in {}\n"
      "\t-> what : `{}`\n"
      "{}",
      when(), location(), where(), what(),
      trace()
    );
  }

private:
  stacktrace m_Trace;
  clock::time_point m_Timestamp;
};


#ifndef Expect
#define Expect(cond, fmt, ...) if (!(cond)) throw CException("Expectation ["#cond"] Failed : "##fmt, __VA_ARGS__);
#endif
