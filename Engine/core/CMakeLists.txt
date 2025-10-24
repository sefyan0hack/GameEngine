
include(helpers)

add_library(core)

target_sources( core PRIVATE
    Window.cpp
    Transform.cpp
    Camera.cpp
    APP.cpp
    GameObject.cpp
    ResourceManager.cpp
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
        Window.hpp
        Transform.hpp
        Camera.hpp
        APP.hpp
        GameObject.hpp
        ResourceManager.hpp
)

target_include_directories(core PRIVATE
    $<$<PLATFORM_ID:Linux>:${X11_INCLUDE_DIR}>
)

target_include_directories(core PRIVATE ${CMAKE_SOURCE_DIR}/Engine)

target_link_libraries(core PUBLIC res)
target_link_libraries(core PRIVATE glm::glm)
target_link_libraries(core PRIVATE stb_image)

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
            ${OPENGL_LIBRARIES}
            $<$<STREQUAL:$<PLATFORM_ID>,Windows>:dbghelp>
            $<$<CXX_COMPILER_ID:GNU>:stdc++exp>
            $<$<PLATFORM_ID:Linux>:dl>
            $<$<PLATFORM_ID:Linux>:${X11_LIBRARIES}>
    )
endif()