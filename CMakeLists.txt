cmake_minimum_required(VERSION 3.25)

project(Engine LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)

# if you have problem with not fouding compiler put path/to/compiler.exe
# Specify the compiler
# SET(CMAKE_SYSTEM_NAME Windows)
# set(CMAKE_C_COMPILER C:/mingw-w64/mingw64/bin/gcc.exe)
# set(CMAKE_CXX_COMPILER C:/mingw-w64/mingw64/bin/g++.exe)

# Print compiler information
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "Compiler Version: ${CMAKE_CXX_COMPILER_VERSION}")


set(CMAKE_MODULE_PATH "${CMAKE_BINARY_DIR}" "${CMAKE_MODULE_PATH}")
set(CMAKE_PREFIX_PATH "${CMAKE_BINARY_DIR}" "${CMAKE_PREFIX_PATH}")

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

include(ProcessorCount)
ProcessorCount(NUMBER_OF_PROCESSORS)
if(NOT NUMBER_OF_PROCESSORS EQUAL 0)
	set(MAKEFLAGS "-j${NUMBER_OF_PROCESSORS}")
	set(ENV{MAKEFLAGS} "${MAKEFLAGS}")
	MESSAGE(STATUS "Set MAKEFLAGS for MinGW Makefiles to ${MAKEFLAGS}")
endif()


MARK_AS_ADVANCED(MULTITHREADED_BUILD)
SET(MULTITHREADED_BUILD ON CACHE BOOL "Parallel build with as many threads as possible!")
if(MULTITHREADED_BUILD)
    if(${CMAKE_GENERATOR} MATCHES "MinGW Makefiles")
        # Use MAKEFLAGS to specify the number of jobs for parallel build
        set(MAKEFLAGS "-j${NUMBER_OF_PROCESSORS}")
        set(ENV{MAKEFLAGS} "${MAKEFLAGS}")
        MESSAGE(STATUS "Set MAKEFLAGS for MinGW Makefiles to ${MAKEFLAGS}")
    elseif(MSVC)
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
        MESSAGE(STATUS "Added parallel build arguments to CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
    endif()
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")

# Find OpenGL package
find_package(OpenGL REQUIRED)

# Add include directorys
include_directories("${CMAKE_SOURCE_DIR}/include")
include_directories(SYSTEM "${CMAKE_SOURCE_DIR}/3party")

# Add glad source file
add_library(glad STATIC 3party/glad/glad.c)
set_target_properties(glad PROPERTIES LINKER_LANGUAGE C)

# Add stb image source file
add_library(stb_image STATIC 3party/stb/stb_image.c)
set_target_properties(stb_image PROPERTIES LINKER_LANGUAGE C)


# remove some warning
add_executable(${PROJECT_NAME} ${SOURCE})
target_link_libraries(${PROJECT_NAME} glad stb_image  ${OPENGL_LIBRARIES})

if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wno-cast-function-type)
    set(CMAKE_CXX_FLAGS_RELEASE " -O3 -march=native") ## Optimsize
    set(CMAKE_CXX_FLAGS_DEBUG   " -pg -g3 -ggdb") ## Debug
endif()