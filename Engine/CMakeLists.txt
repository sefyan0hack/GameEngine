
include(helpers)

add_library(core)

target_sources( core PRIVATE
        src/core/Keyboard.cpp
        src/core/Mouse.cpp
        src/core/Window.cpp
        src/core/Renderer.cpp
        src/core/Shader.cpp
        src/core/Transform.cpp
        src/core/Camera.cpp
        src/core/Mesh.cpp
        src/core/OpenGL.cpp
        src/core/APP.cpp
        src/core/GameObject.cpp
        src/core/Texture.cpp
        src/core/Scene.cpp
        src/core/Material.cpp
        src/core/ResourceManager.cpp
        src/core/Image.cpp
        src/core/ShaderProgram.cpp
        src/core/SkyBox.cpp
)

target_sources( core PUBLIC
    FILE_SET HEADERS
      TYPE HEADERS
      FILES
        include/core/Platform.hpp
        include/core/Log.hpp
        include/core/Utils.hpp
        include/core/gl.h
        include/core/fmts.hpp
        include/core/Event.hpp
        include/core/EventQueue.hpp
        include/core/Function.hpp
        include/core/AutoRelease.hpp
        include/core/ResourceManager.hpp
        include/core/Keyboard.hpp
        include/core/Mouse.hpp
        include/core/Window.hpp
        include/core/Renderer.hpp
        include/core/Shader.hpp
        include/core/Transform.hpp
        include/core/Camera.hpp
        include/core/Mesh.hpp
        include/core/OpenGL.hpp
        include/core/APP.hpp
        include/core/GameObject.hpp
        include/core/Texture.hpp
        include/core/Scene.hpp
        include/core/Material.hpp
        include/core/ResourceManager.hpp
        include/core/Image.hpp
        include/core/ShaderProgram.hpp
        include/core/SkyBox.hpp
)

target_include_directories(core PUBLIC
    "${CMAKE_SOURCE_DIR}/Engine/include"
    $<$<PLATFORM_ID:Linux>:${X11_INCLUDE_DIR}>
)

target_link_libraries(core PUBLIC res)
target_link_libraries(core PRIVATE glm::glm)
target_link_libraries(core PRIVATE stb_image)

apply_all_options(TARGETS core)
apply_coverage_options(TARGETS core)
apply_harden_options(TARGETS core)
apply_sanitizer_options(TARGETS core)


target_compile_definitions( core PUBLIC
    PLATFORM_NAME="${CMAKE_SYSTEM_NAME}"
    PLATFORM_VERSION="${CMAKE_SYSTEM_VERSION}"
    PLATFORM_ARCH="${CMAKE_SYSTEM_PROCESSOR}"

    OPENGL_LIB="${CORE_OPENGL_LIB}"

    $<$<PLATFORM_ID:Windows>:WINDOWS_PLT>
    $<$<PLATFORM_ID:Linux>:LINUX_PLT>
    $<$<PLATFORM_ID:Emscripten>:WEB_PLT>

    $<$<CXX_COMPILER_ID:GNU>:GNU_CPL>
    $<$<CXX_COMPILER_ID:Clang>:CLANG_CPL>
    $<$<CXX_COMPILER_ID:MSVC>:MSVC_CPL>

    $<$<CONFIG:Debug>:DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
)

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

add_library(Engine INTERFACE)

target_link_libraries(Engine
    INTERFACE
        core
)

print_target_compile_options(Engine)
