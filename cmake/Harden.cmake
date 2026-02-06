add_library(project_hardening_flags INTERFACE)

if(HARDEN)
    target_compile_options(project_hardening_flags INTERFACE
        $<$<AND:$<CXX_COMPILER_ID:GNU>,$<NOT:$<PLATFORM_ID:Windows>>>:-fhardened>
        $<$<CXX_COMPILER_ID:Clang>:-D_LIBCPP_HARDENING_MODE=_LIBCPP_HARDENING_MODE_EXTENSIVE>
        $<$<CXX_COMPILER_ID:MSVC>:-D_MSVC_STL_HARDENING=1>
    )
endif()