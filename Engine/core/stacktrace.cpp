#include "stacktrace.hpp"
#include "Log.hpp"

#if defined(WINDOWS_PLT)
#include <windows.h>
#endif

auto to_string(const stacktrace& st ) -> std::string
{
  std::string res = std::format("Stack Trace ({} Frames): {{\n", st.size());
  res += std::format("\t{:<3} {:<18} {:<40} {}\n", "ID", "Address", "Function", "Location");
  res += "\t--------------------------------------------------------------------------------\n";
  size_t idx = 1;
  for(const auto& frame : st){
    res += std::format("\t{:<3} {:<18p} {:<40} at {}:{}\n", idx++, (void*)frame.native_handle(), frame.description(), frame.source_file(), frame.source_line());
  }
  res += "}\n";
  return res;
}
