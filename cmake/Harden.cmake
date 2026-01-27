add_library(project_hardening_flags INTERFACE)

if(HARDEN)
    target_compile_options(project_hardening_flags INTERFACE
        $<$<AND:$<CXX_COMPILER_ID:GNU>,$<NOT:$<PLATFORM_ID:Windows>>>:-fhardened>
        $<$<CXX_COMPILER_ID:GNU,Clang>:
            $<$<PLATFORM_ID:Windows>:-fstack-protector-strong -D_FORTIFY_SOURCE=2>
            -Wformat -Wformat-security -Werror=format-security
        >

        # --- MSVC ---
        $<$<CXX_COMPILER_ID:MSVC>:
            /sdl            # Enable Additional Security Checks
            /GS             # Buffer Security Check
            /guard:cf       # Control Flow Guard
            /W4 /WX         # Warnings as Errors
        >
    )

    # 3. Add Hardened Linker Options
    target_link_options(project_hardening_flags INTERFACE
        # --- MSVC Linker ---
        $<$<CXX_COMPILER_ID:MSVC>:
            /DYNAMICBASE    # ASLR
            /HIGHENTROPYVA  # 64-bit ASLR
            /NXCOMPAT       # Data Execution Prevention (DEP)
        >
    )

    # 4. Standard Library Hardening Definitions
    target_compile_definitions(project_hardening_flags INTERFACE
        # libstdc++ (GCC) assertions
        $<$<CXX_COMPILER_ID:GNU>:_GLIBCXX_ASSERTIONS>
        
        # libc++ (Clang) extensive hardening (replaces older modes)
        $<$<CXX_COMPILER_ID:Clang>:_LIBCPP_HARDENING_MODE=_LIBCPP_HARDENING_MODE_EXTENSIVE>
    )
endif()