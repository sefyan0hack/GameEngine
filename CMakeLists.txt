cmake_minimum_required(VERSION 3.25)

project(Engine LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)

message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} (${CMAKE_CXX_COMPILER_VERSION})")

include(${CMAKE_SOURCE_DIR}/cmake/helpers.cmake)

# Include get_cpm.cmake
include(${CMAKE_SOURCE_DIR}/cmake/get_cpm.cmake)

# deps :

    # add glm 1.0.1
CPMAddPackage("gh:g-truc/glm#1.0.1")

    # stb_image v2.30
CPMAddPackage(
    NAME stb_image_header
    URL https://raw.githubusercontent.com/nothings/stb/5c205738c191bcb0abc65c4febfa9bd25ff35234/stb_image.h
    DOWNLOAD_ONLY YES
    DOWNLOAD_NO_EXTRACT TRUE
    DOWNLOAD_NAME stb_image.h
)

    # glad c/c++ gl 4.4
CPMAddPackage(
    NAME glad
    URL https://glad.dav1d.de/generated/tmpjukken7yglad/glad.zip
)

if(stb_image_header_ADDED)
  # Create a dummy source file to compile the implementation
  file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/stb_image.cpp"
    "#define STB_IMAGE_IMPLEMENTATION\n#include \"stb_image.h\"\n"
  )
endif()

# Print compiler information

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

add_executable(${PROJECT_NAME} ${SOURCE})

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/include" )

if(MSVC)
    target_link_libraries(${PROJECT_NAME} glad stb_image glm::glm ${OPENGL_LIBRARIES} )
else()
    target_link_libraries(${PROJECT_NAME} glad stb_image glm::glm ${OPENGL_LIBRARIES} stdc++exp )

endif()

# glad lib
add_library(glad STATIC "${glad_SOURCE_DIR}/src/glad.c")
target_include_directories(glad PUBLIC "${glad_SOURCE_DIR}/include")
set_target_properties(glad PROPERTIES LINKER_LANGUAGE C)

# stb_image lib
add_library(stb_image STATIC "${CMAKE_CURRENT_BINARY_DIR}/stb_image.cpp")
target_include_directories(stb_image PUBLIC ${stb_image_header_SOURCE_DIR})
set_target_properties(stb_image PROPERTIES LINKER_LANGUAGE C)

# flags
apply_compile_options(TARGETS ${PROJECT_NAME})

print_target_compile_options(${PROJECT_NAME})

target_compile_definitions(${PROJECT_NAME}
    PRIVATE
        PROJECT_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
)