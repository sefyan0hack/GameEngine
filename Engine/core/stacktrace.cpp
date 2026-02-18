#include "stacktrace.hpp"
#include <format>

auto to_string(const stacktrace_entry& ste ) -> std::string
{
  return std::format("{:<18p} {:<40} at {}:{}", (void*)ste.native_handle(), ste.description(), ste.source_file(), ste.source_line());
}

auto to_string(const stacktrace& st ) -> std::string
{
  std::string res = std::format("Stack Trace ({} Frames): {{\n", st.size());
  for(size_t idx = 1; const auto& frame : st){
    res += std::format("\t{:<3} {}\n", idx++, ::to_string(frame));
  }
  res += "}\n";
  return res;
}
