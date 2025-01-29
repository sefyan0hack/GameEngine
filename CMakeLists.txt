cmake_minimum_required(VERSION 3.25)

project(Engine LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)

include(${CMAKE_SOURCE_DIR}/cmake/helpers.cmake)

# Include get_cpm.cmake
include(${CMAKE_SOURCE_DIR}/cmake/get_cpm.cmake)

# deps :
CPMAddPackage("gh:g-truc/glm#1.0.1") # add glm 1.0.1


# Print compiler information
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} (${CMAKE_CXX_COMPILER_VERSION})")

set(SOURCE 
    src/main.cpp
    src/Keyboard.cpp
    src/Mouse.cpp
	src/Window.cpp
	src/Renderer.cpp
	src/Shader.cpp
	src/Transform.cpp
    src/Mesh.cpp
    src/Camera.cpp
    src/OpenGL.cpp
    src/APP.cpp
    src/Utils.cpp
    src/GameObject.cpp
    src/Texture.cpp
    src/Scene.cpp
    src/Material.cpp
	)


# Find OpenGL package
find_package(OpenGL REQUIRED)

# Add include directorys
include_directories(SYSTEM "${CMAKE_SOURCE_DIR}/3party")


add_executable(${PROJECT_NAME} ${SOURCE})

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/include" )

if(MSVC)
    target_link_libraries(${PROJECT_NAME} glad stb_image glm::glm ${OPENGL_LIBRARIES} )
else()
    target_link_libraries(${PROJECT_NAME} glad stb_image glm::glm ${OPENGL_LIBRARIES} stdc++exp )

endif()


# Add glad source file
add_library(glad STATIC 3party/glad/glad.c)
set_target_properties(glad PROPERTIES LINKER_LANGUAGE C)

# Add stb image source file
add_library(stb_image STATIC 3party/stb/stb_image.c)
set_target_properties(stb_image PROPERTIES LINKER_LANGUAGE C)

# flags
apply_compile_options(TARGETS ${PROJECT_NAME} glad stb_image glm::glm)

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 "$<$<CONFIG:Debug>:/analyze>")
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wno-cast-function-type)
endif()

print_target_compile_options(${PROJECT_NAME})
print_target_compile_options(glad)
print_target_compile_options(stb_image)
print_target_compile_options(glm::glm)

target_compile_definitions(${PROJECT_NAME}
    PRIVATE
        PROJECT_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
)