
if(LINK_TIME_OPT)
    include(CheckIPOSupported)
    check_ipo_supported(RESULT IPO_SUPPORTED OUTPUT OUTPUT)

    if(IPO_SUPPORTED AND NOT TESTS)
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
    endif()

    add_compile_options(
        "$<$<CXX_COMPILER_ID:GNU,Clang>:-ffunction-sections;-fdata-sections>"
        "$<$<CXX_COMPILER_ID:GNU,Clang>:-funwind-tables>" # generate some exception data (usually disabled for C programs)
        "$<$<CXX_COMPILER_ID:GNU>:-flto-partition=none>"
        "$<$<CXX_COMPILER_ID:MSVC>:/Gy;/Gw>"
        )
        
        add_link_options(
            "$<$<CXX_COMPILER_ID:GNU>:-flto-partition=none>"
            "$<$<CXX_COMPILER_ID:GNU,Clang>:-fuse-linker-plugin>"
            "$<$<CXX_COMPILER_ID:MSVC>:/OPT:REF;/OPT:ICF>"
    )

    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        # GCC requires gcc-ar/gcc-ranlib to handle LTO objects
        find_program(GCC_AR NAMES "${CMAKE_CXX_COMPILER_PREFIX}gcc-ar" gcc-ar)
        find_program(GCC_RANLIB NAMES "${CMAKE_CXX_COMPILER_PREFIX}gcc-ranlib" gcc-ranlib)
        find_program(GCC_NM NAMES "${CMAKE_CXX_COMPILER_PREFIX}gcc-nm" gcc-nm)

        if(GCC_AR AND GCC_RANLIB)
            set(CMAKE_AR ${GCC_AR})
            set(CMAKE_RANLIB ${GCC_RANLIB})
            set(CMAKE_NM ${GCC_NM})
            set(CMAKE_CXX_COMPILER_AR ${GCC_AR})
            set(CMAKE_CXX_COMPILER_RANLIB ${GCC_RANLIB})
        endif()

    elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        # Clang uses LLVM bitcode, which standard 'ar' cannot read
        find_program(LLVM_AR NAMES "${CMAKE_CXX_COMPILER_PREFIX}llvm-ar" llvm-ar)
        find_program(LLVM_RANLIB NAMES "${CMAKE_CXX_COMPILER_PREFIX}llvm-ranlib" llvm-ranlib)
        find_program(LLVM_NM NAMES "${CMAKE_CXX_COMPILER_PREFIX}llvm-nm" llvm-nm)

        if(LLVM_AR AND LLVM_RANLIB)
            set(CMAKE_AR ${LLVM_AR})
            set(CMAKE_RANLIB ${LLVM_RANLIB})
            set(CMAKE_NM ${LLVM_NM})
            set(CMAKE_CXX_COMPILER_AR ${LLVM_AR})
            set(CMAKE_CXX_COMPILER_RANLIB ${LLVM_RANLIB})
        endif()

    elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
        # MSVC handles LTO via /LTCG and lib.exe automatically.
        # No tool swapping is needed
    endif()
endif()
