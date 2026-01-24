#pragma once

#ifdef __has_include
  #if __has_include(<format>)
    #include <format>
    #ifndef __cpp_lib_formatters
    #error "__cpp_lib_formatters not def"
    #endif
  #endif
#endif