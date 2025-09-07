
include(helpers)

add_library(core)

target_sources( core PRIVATE
    Keyboard.cpp
    Mouse.cpp
    Window.cpp
    Renderer.cpp
    Shader.cpp
    Transform.cpp
    Camera.cpp
    Mesh.cpp
    OpenGL.cpp
    APP.cpp
    GameObject.cpp
    Texture.cpp
    Scene.cpp
    Material.cpp
    ResourceManager.cpp
    Image.cpp
    ShaderProgram.cpp
    SkyBox.cpp
)

target_sources( core PUBLIC
    FILE_SET HEADERS
      TYPE HEADERS
      FILES
        Platform.hpp
        Log.hpp
        Utils.hpp
        gl.h
        fmts.hpp
        Event.hpp
        EventQueue.hpp
        Function.hpp
        ResourceManager.hpp
        Keyboard.hpp
        Mouse.hpp
        Window.hpp
        Renderer.hpp
        Shader.hpp
        Transform.hpp
        Camera.hpp
        Mesh.hpp
        OpenGL.hpp
        APP.hpp
        GameObject.hpp
        Texture.hpp
        Scene.hpp
        Material.hpp
        ResourceManager.hpp
        Image.hpp
        ShaderProgram.hpp
        SkyBox.hpp
)

target_include_directories(core PRIVATE
    $<$<PLATFORM_ID:Linux>:${X11_INCLUDE_DIR}>
)

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