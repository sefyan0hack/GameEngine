
if(ASAN)
    if(MSVC)
        set(MSVC_SANITIZER_FLAGS /fsanitize=address)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        set(CLANG_GNU_SANITIZER_FLAGS -fsanitize=address)
    endif()
    
endif()

if(USAN)
    if(MSVC)
        message(STATUS "USAN NOT WORKING IN `MSVC` !!")
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        set(CLANG_GNU_SANITIZER_FLAGS -fsanitize=undefined)
    endif()
endif()

if(TSAN)
    if(MSVC)
        message(STATUS "TSAN NOT WORKING IN `MSVC` !!")
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        set(CLANG_GNU_SANITIZER_FLAGS -fsanitize=thread)
    endif()
endif()

add_library(project_sanitizer_flags INTERFACE)

target_compile_options(project_sanitizer_flags INTERFACE
    $<$<CXX_COMPILER_ID:Clang,GNU>:${CLANG_GNU_SANITIZER_FLAGS}>
)

target_link_options(project_sanitizer_flags INTERFACE
    $<$<CXX_COMPILER_ID:Clang,GNU>:${CLANG_GNU_SANITIZER_FLAGS}>
)

if(ASAN OR USAN OR TSAN AND NOT WIN32 )
    target_link_options(project_sanitizer_flags INTERFACE
        $<$<CXX_COMPILER_ID:GNU>:-static-libasan;-static-libubsan;-static-liblsan>
        $<$<CXX_COMPILER_ID:Clang>:-static-libsan>
    )
endif()