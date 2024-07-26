#pragma once
#pragma GCC system_header
// LOG & ERR 
#ifndef _GLIBCXX_IOSTREAM
    #include <iostream>
#endif

#if !defined(ERR) && !defined(LOG)
    #define LOG(str) do { std::cout << "LOG : " << str << "\n"; } while(false)
    #define ERR(str) do { std::cerr << "ERROR : " << __FILE__ << ":" << __LINE__  << "  ::  \" "<< str << " \"\n"; exit(EXIT_FAILURE); } while(false)
#else
    #error "LOG && ERR allredy defined"
#endif

// unused warning 
#ifndef UNISED
    #define UNUSED(x) (void)x;
#else
    #error "UNUSED alredy defined"
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
// random float
#define Rand_float static_cast<float>(rand()) / static_cast<float>(RAND_MAX)

//
// remember macro 'SHADER' replace full path woth relative
#define SHADER(str)  "C:/Users/sefyan/Documents/c_projects/test/Learn-Opengl/Shasers/"#str
