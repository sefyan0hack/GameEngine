#pragma once

#if __cpp_lib_stacktrace

#include <stacktrace>

using stacktrace = std::stacktrace;

#else

#if defined(WEB_PLT)
#include <emscripten/emscripten.h>
#endif

struct stacktrace {
  static stacktrace current() noexcept { return {}; }
  static stacktrace current([[maybe_unused]] const size_t skip) noexcept { return {}; }
  size_t size() const { return 0; }
};

inline auto to_string([[maybe_unused]] const stacktrace& st ) -> std::string
{
#if defined(WEB_PLT)
    return EM_ASM_STRING({ return new Error().stack; });
#else
    return "no_stack_trace";
#endif
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