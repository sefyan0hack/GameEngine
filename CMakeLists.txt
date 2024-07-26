cmake_minimum_required(VERSION 3.30)

project(test LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)

# Specify the compiler
SET(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_C_COMPILER C:/mingw-w64/mingw64/bin/gcc.exe)
set(CMAKE_CXX_COMPILER C:/mingw-w64/mingw64/bin/g++.exe)


set(CMAKE_MODULE_PATH "${CMAKE_BINARY_DIR}" "${CMAKE_MODULE_PATH}")
set(CMAKE_PREFIX_PATH "${CMAKE_BINARY_DIR}" "${CMAKE_PREFIX_PATH}")

set(SOURCE 
	src/main.cpp
	src/Window.cpp
	src/Renderer.cpp
	src/Shader.cpp
	src/VAO.cpp
	src/VBO.cpp
	)


set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")

# Find OpenGL package
find_package(OpenGL REQUIRED)


# Add glad include directory
include_directories("${PROJECT_SOURCE_DIR}/3party")

# Add glad source file
add_library(glad STATIC 3party/glad/glad.c)
set_target_properties(glad PROPERTIES LINKER_LANGUAGE C)

# remove some warning
set(REMOVE_WAR "-Wno-cast-function-type")
if (MSVC)
add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
    set(CMAKE_CXX_FLAGS_RELEASE "${REMOVE_WAR} -Ofast -march=native") ## Optimize
    set(CMAKE_CXX_FLAGS_DEBUG   "${REMOVE_WAR} -O0 -g3 -ggdb") ## Debug
endif()
add_executable(test  ${SOURCE})
target_link_libraries(test glad  ${OPENGL_LIBRARIES})
