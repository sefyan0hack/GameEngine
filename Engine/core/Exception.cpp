#include "Exception.hpp"


auto Exception::what() const noexcept  -> const char* {
  return std::runtime_error::what();
}

auto Exception::trace() const noexcept -> std::string {
  return to_string(m_Trace);
}

auto Exception::where() const noexcept -> std::string {
  return !m_Trace.empty() ? m_Trace.begin()->description() : "??";
}

auto Exception::location() const noexcept -> std::string {
  return !m_Trace.empty() ? std::format("{}:{}", m_Trace.begin()->source_file(), m_Trace.begin()->source_line()) : "??:??";
}

auto Exception::all() const -> std::string
{
  return std::format(
    "(Exception) at [{}] in {}\n"
    "\t-> what : `{}`\n"
    "{}",
    location(), where(), what(),
    trace()
  );
}