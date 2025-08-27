
include(helpers)


set(CORE_SOURCES
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

add_library(core OBJECT ${CORE_SOURCES})
target_link_libraries(core PUBLIC res)

add_library( Engine 
    $<TARGET_OBJECTS:core>
)
target_link_libraries(Engine PUBLIC res)

target_compile_options(Engine PRIVATE -flto)
target_link_options(Engine PRIVATE -flto)

apply_all_options(TARGETS Engine)
apply_coverage_options(TARGETS Engine)
apply_harden_options(TARGETS Engine)
apply_sanitizer_options(TARGETS Engine)

####################################################################################################
# Link Libs 
####################################################################################################
# common Libs

target_link_libraries(Engine
    PRIVATE
        glm::glm
        stb_image
)

# common (Not Web)
if(NOT EMSCRIPTEN)
    if(NOT WIN32)
    target_link_libraries(Engine PRIVATE $<$<CXX_COMPILER_ID:Clang>:stdc++exp>)
    endif()

    target_link_libraries(Engine
        PRIVATE
            Threads::Threads
            ${OPENGL_LIBRARIES}
            $<$<STREQUAL:$<PLATFORM_ID>,Windows>:dbghelp>
            $<$<CXX_COMPILER_ID:GNU>:stdc++exp>
            $<$<PLATFORM_ID:Linux>:dl>
            $<$<PLATFORM_ID:Linux>:${X11_LIBRARIES}>
    )
endif()

print_target_compile_options(Engine)
