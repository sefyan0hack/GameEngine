
set(MSVC_WARNING_FLAGS 
    /W4 /permissive- /wd4251 /wd4275
)

set(GNU_WARNING_FLAGS
    -pedantic -Wno-cast-function-type -Winit-self -Wcast-qual
    -Wsuggest-final-types -Wsuggest-final-methods -Wno-changes-meaning
)

set(CLANG_WARNING_FLAGS 
    -Wno-language-extension-token
)

set(CLANG_AND_GNU_WARNING_FLAGS
    -Wall -Wextra -Wpedantic -Wconversion -Wfloat-equal -Wwrite-strings -Wno-sign-conversion -Werror=null-dereference 
    -Wnull-dereference -Wswitch-enum -Wuninitialized -Wpointer-arith -Wreturn-type -Wredundant-decls -fno-operator-names
)

add_library(project_warnings_flags INTERFACE)

target_compile_options(project_warnings_flags INTERFACE
    $<$<CXX_COMPILER_ID:MSVC>:${MSVC_WARNING_FLAGS}>
    $<$<CXX_COMPILER_ID:GNU>:${CLANG_AND_GNU_WARNING_FLAGS};${GNU_WARNING_FLAGS}>
    $<$<CXX_COMPILER_ID:Clang>:${CLANG_AND_GNU_WARNING_FLAGS};${CLANG_WARNING_FLAGS}>
)