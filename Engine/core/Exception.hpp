#pragma once
#include <exception>
#include <source_location>
#include <format>
#include <engine_export.h>
#include "stacktrace.hpp"


class ENGINE_API Exception final : public std::runtime_error {
public:

  template <typename... Ts>
  Exception(
    const std::format_string<Ts...>& fmt,
    Ts&&... args
  )
    : std::runtime_error(std::format(fmt, std::forward<Ts>(args)...))
    , m_Trace(stacktrace::current(1))
  {}

  auto what() const noexcept  -> const char* override;
  auto trace() const noexcept -> std::string;
  auto where() const noexcept -> std::string;
  auto location() const noexcept -> std::string;

  auto all() const -> std::string;

private:
  stacktrace m_Trace;
};

#ifndef Try
#define Try(f) try {f;} catch(...){}
#endif

#ifndef Expect
#define Expect(cond, ...) if (!(cond)) throw Exception("Expectation ["#cond"] Failed : " __VA_ARGS__);
#endif