#pragma once
#include <string>
#include <array>
#include <cstdint>
#include <engine_export.h>

#if __cpp_lib_stacktrace

#include <stacktrace>

using stacktrace = std::stacktrace;
using stacktrace_entry = std::stacktrace_entry;

auto ENGINE_EXPORT to_string(const stacktrace& st ) -> std::string;
#else

struct ENGINE_EXPORT stacktrace_entry {
  [[nodiscard]] constexpr std::uintptr_t native_handle() const noexcept { return {}; }
  [[nodiscard]] constexpr std::string description() const { return "??"; }
  [[nodiscard]] constexpr std::string source_file() const { return "??"; }
  [[nodiscard]] constexpr uint32_t source_line() const { return {}; }
};

struct ENGINE_EXPORT stacktrace {
  using const_iterator = std::array<stacktrace_entry, 0>::const_iterator;
  
  static stacktrace current() noexcept { return {}; }
  static stacktrace current([[maybe_unused]] const size_t skip) noexcept { return {}; }
  
  // Required members
  bool empty() const noexcept { return m_entries.empty(); }
  const_iterator begin() const noexcept { return m_entries.begin(); }
  const_iterator end() const noexcept { return m_entries.end(); }
  size_t size() const noexcept { return m_entries.size(); }
  
  const stacktrace_entry& operator[](size_t index) const { return m_entries[index]; }

private:
  std::array<stacktrace_entry, 0> m_entries;
};

inline auto to_string([[maybe_unused]] const stacktrace& st ) -> std::string
{
  return std::format("Stack Trace ({} Frame): {{ }}", st.size());
}

template<>
struct std::formatter<stacktrace> {
  constexpr auto parse(std::format_parse_context& context) {
    return context.begin();
  }
  auto format(const stacktrace& obj, std::format_context& context) const {
    return std::format_to(context.out(),"{}", to_string(obj));
  }
};

#endif