#pragma once
#include <string>

#if __cpp_lib_stacktrace

#include <stacktrace>

using stacktrace = std::stacktrace;

inline auto to_string([[maybe_unused]] const stacktrace& st ) -> std::string
{
  std::string res = std::format("Stack Trace ({} Frame): {{\n", st.size());
  size_t idx = 0;
  for(const auto& frame : st){
     res += std::format("\t{}# {} at {}:{}\n", idx++, frame.description(), frame.source_file(), frame.source_line());
  }
  res += "}}\n";
  return res;
}
#else

struct stacktrace {
  static stacktrace current() noexcept { return {}; }
  static stacktrace current([[maybe_unused]] const size_t skip) noexcept { return {}; }
  size_t size() const { return 0; }
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