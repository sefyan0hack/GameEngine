
include(helpers)

add_library(core)

target_sources( core PRIVATE
    APP.cpp
)

target_sources( core PUBLIC
    FILE_SET HEADERS
      TYPE HEADERS
      FILES
        core.hpp
        Platform.hpp
        Log.hpp
        Utils.hpp
        fmts.hpp
        Event.hpp
        EventQueue.hpp
        Function.hpp
        ResourceManager.hpp
        APP.hpp
        ResourceManager.hpp
        DynLib.hpp
)

target_include_directories(core 
    INTERFACE 
        ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE 
        ${CMAKE_SOURCE_DIR}/Engine
)

apply_all_options(TARGETS core)
apply_coverage_options(TARGETS core)
apply_harden_options(TARGETS core)
apply_sanitizer_options(TARGETS core)

# common (Not Web)
if(NOT EMSCRIPTEN)
    if(NOT WIN32)
    target_link_libraries(core PRIVATE $<$<CXX_COMPILER_ID:Clang>:stdc++exp>)
    endif()

    target_link_libraries(core
        PRIVATE
            Threads::Threads
            $<$<CXX_COMPILER_ID:GNU>:stdc++exp>
            $<$<PLATFORM_ID:Linux>:dl>
    )
endif()