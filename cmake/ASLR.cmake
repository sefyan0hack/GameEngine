
if(ASLR)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        add_link_options("-Wl,--dynamicbase")
        add_link_options("-Wl,--high-entropy-va")
        add_link_options("-Wl,--nxcompat") # Data Execution Prevention
    elseif(MSVC)
        add_link_options("/DYNAMICBASE")
        add_link_options("/HIGHENTROPYVA")
        add_link_options("/NXCOMPAT")
    endif()
else()   
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        # Disable ASLR
        add_link_options("-Wl,--disable-dynamicbase")
        add_link_options("-Wl,--disable-high-entropy-va")
        
        add_compile_options("-fno-omit-frame-pointer") # Essential for stack traces
        
        if(MINGW)
            add_compile_options("-fno-optimize-sibling-calls")
            add_compile_options("-fno-inline")
            add_compile_options("-fno-eliminate-unused-debug-types") 
            add_compile_options("-fno-merge-debug-strings")
        endif()

    elseif(MSVC)
        # Disable ASLR
        add_link_options("/DYNAMICBASE:NO")
        add_link_options("/HIGHENTROPYVA:NO")
    endif()
endif()