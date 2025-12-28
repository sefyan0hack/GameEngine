#pragma once
#include <exception>
#include <source_location>
#include <format>
#include <engine_export.h>
#include "stacktrace.hpp"


class ENGINE_EXPORT Exception final : public std::exception{
public:

  template <typename... Ts>
  Exception(
    const std::format_string<Ts...>& fmt,
    Ts&&... args
  ) : m_Trace(stacktrace::current(1))
  {
    std::string user_msg = std::format(fmt, std::forward<Ts>(args)...);
     
    m_FullMessage = std::format(
        "(Exception) at [{}] in {}\n"
        "\t-> {}\n"
        "{}",
        location(), where(), user_msg, trace()
    );
  }

  auto what() const noexcept  -> const char* override;
  auto trace() const noexcept -> std::string;
  auto where() const noexcept -> std::string;
  auto location() const noexcept -> std::string;

private:
  std::string m_FullMessage;
  stacktrace m_Trace;
};

#ifndef Try
#define Try(f) try {f;} catch(...){}
#endif

#ifndef Expect
#define Expect(cond, ...) if (!(cond)) [[unlikely]] throw Exception("Expectation ["#cond"] Failed : " __VA_ARGS__);
#endif