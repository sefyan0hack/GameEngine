#pragma once
#if !defined _MSC_VER
#pragma GCC system_header
#endif

// disable warning
#ifdef _MSC_VER
    #define NO_WARNING_BEGIN \
        __pragma(warning(push, 0))
    #define NO_WARNING_END \
        __pragma(warning(pop))
#elif defined(__GNUC__)
    #define NO_WARNING_BEGIN \
        _Pragma("GCC diagnostic push") \
        _Pragma("GCC diagnostic ignored \"-Wall\"") \
        _Pragma("GCC diagnostic ignored \"-Wextra\"") \
        _Pragma("GCC diagnostic ignored \"-Wpedantic\"") \
        _Pragma("GCC diagnostic ignored \"-Wconversion\"") \
        _Pragma("GCC diagnostic ignored \"-Wshadow\"") \
        _Pragma("GCC diagnostic ignored \"-Wcast-align\"") \
        _Pragma("GCC diagnostic ignored \"-Wunused-parameter\"") \
        _Pragma("GCC diagnostic ignored \"-w\"")
    #define NO_WARNING_END \
        _Pragma("GCC diagnostic pop")
#else
    #define NO_WARNING_BEGIN
    #define NO_WARNING_END
#endif

#ifdef PROJECT_SOURCE_DIR
    #define TEXTURE(str)  PROJECT_SOURCE_DIR"/res/"#str
    #define SHADER(str)   PROJECT_SOURCE_DIR"/res/Shaders/"#str
#else
    #define TEXTURE(str) #str
    #define SHADER(str)  #str
#endif