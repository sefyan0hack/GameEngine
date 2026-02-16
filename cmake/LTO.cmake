
if(LTO)
    if(COVERAGE)
        message(STATUS "LTO with Coverage not allowed")
        return()
    endif()
    # Ensure IPO/LTO policy is modern
    cmake_policy(SET CMP0069 NEW)

    include(CheckIPOSupported)
    check_ipo_supported(RESULT IPO_SUPPORTED OUTPUT OUTPUT LANGUAGES CXX)

    if(IPO_SUPPORTED)
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
    endif()
endif()
