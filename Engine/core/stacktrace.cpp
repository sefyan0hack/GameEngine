#include "stacktrace.hpp"
#include "Log.hpp"

#if defined(WINDOWS_PLT)
#include <windows.h>
#endif


auto to_string(const stacktrace_entry& ste ) -> std::string
{
  return ::format("{:<18p} {:<40} at {}:{}", (void*)ste.native_handle(), ste.description(), ste.source_file(), ste.source_line());
}

auto to_string(const stacktrace& st ) -> std::string
{
  std::string res = ::format("Stack Trace ({} Frames): {{\n", st.size());
  for(size_t idx = 1; const auto& frame : st){
    res += ::format("\t{:<3} {}\n", idx++, ::to_string(frame));
  }
  res += "}\n";
  return res;
}
