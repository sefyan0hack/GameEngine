set(CLANG_GNU_SANITIZER_FLAGS "")

if(ASAN)
    if(MSVC)
        set(MSVC_SANITIZER_FLAGS "/fsanitize=address")
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        list(APPEND CLANG_GNU_SANITIZER_FLAGS "-fsanitize=address")
    endif()
endif()

if(USAN)
    if(MSVC)
        message(STATUS "UBSan NOT supported in MSVC (AddressSanitizer only)")
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        list(APPEND CLANG_GNU_SANITIZER_FLAGS "-fsanitize=undefined")
    endif()
endif()


add_library(project_sanitizer_flags INTERFACE)

target_compile_options(project_sanitizer_flags INTERFACE
    $<$<CXX_COMPILER_ID:Clang,GNU>:${CLANG_GNU_SANITIZER_FLAGS}>
    $<$<CXX_COMPILER_ID:MSVC>:${MSVC_SANITIZER_FLAGS}>
)

target_link_options(project_sanitizer_flags INTERFACE
    $<$<CXX_COMPILER_ID:Clang,GNU>:${CLANG_GNU_SANITIZER_FLAGS}>
    $<$<CXX_COMPILER_ID:MSVC>:${MSVC_SANITIZER_FLAGS}>
)

if((ASAN OR USAN OR TSAN) AND NOT WIN32)
    target_link_options(project_sanitizer_flags INTERFACE
        $<$<CXX_COMPILER_ID:GNU>:-static-libasan;-static-libubsan>
    )
endif()