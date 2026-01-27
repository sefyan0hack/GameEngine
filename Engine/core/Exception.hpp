#pragma once
#include <exception>
#include <source_location>
#include "format.hpp"
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
    std::string user_msg = ::format(fmt, std::forward<Ts>(args)...);
     
    m_FullMessage = ::format(
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


template<class... Args>
inline void Expect(bool cond, std::format_string<Args...> fmt = {}, Args&&... args)
{
  if (!cond) [[unlikely]]
    throw Exception("Expectation Failed. {}", ::format(fmt, std::forward<Args>(args)...));
}