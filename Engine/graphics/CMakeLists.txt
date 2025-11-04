
include(helpers)

add_library(graphics)

target_sources( graphics PRIVATE
    Window.cpp    
    Shader.cpp
    ShaderProgram.cpp
    Mesh.cpp
    OpenGL.cpp
    Texture.cpp
    Scene.cpp
    Material.cpp
    SkyBox.cpp
    Image.cpp
    Transform.cpp
    Camera.cpp
    GameObject.cpp
    OpenGLRenderer.cpp
)

target_sources( graphics PUBLIC
    FILE_SET HEADERS
      TYPE HEADERS
      FILES
        gl.h
        Window.hpp
        Renderer.hpp
        Shader.hpp
        ShaderProgram.hpp
        Mesh.hpp
        OpenGL.hpp
        Texture.hpp
        Scene.hpp
        Material.hpp
        Image.hpp
        SkyBox.hpp
        Transform.hpp
        Camera.hpp
        GameObject.hpp
        GApi.hpp
        OpenGLRenderer.hpp
)

target_include_directories(core PRIVATE
    $<$<PLATFORM_ID:Linux>:${X11_INCLUDE_DIR}>
)

target_include_directories(graphics PRIVATE ${CMAKE_SOURCE_DIR}/Engine)

target_link_libraries(graphics PUBLIC res)
target_link_libraries(graphics PRIVATE glm::glm)
target_link_libraries(graphics PRIVATE stb_image)

apply_all_options(TARGETS graphics)
apply_coverage_options(TARGETS graphics)
apply_harden_options(TARGETS graphics)
apply_sanitizer_options(TARGETS graphics)

# common (Not Web)
if(NOT EMSCRIPTEN)
    if(NOT WIN32)
    target_link_libraries(graphics PRIVATE $<$<CXX_COMPILER_ID:Clang>:stdc++exp>)
    endif()

    target_link_libraries(graphics
        PRIVATE
            Threads::Threads
            ${OPENGL_LIBRARIES}
            $<$<CXX_COMPILER_ID:GNU>:stdc++exp>
            $<$<PLATFORM_ID:Linux>:dl>
            $<$<PLATFORM_ID:Linux>:${X11_LIBRARIES}>

    )
endif()