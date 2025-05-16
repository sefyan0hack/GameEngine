cmake_minimum_required(VERSION 3.28)

project(Engine LANGUAGES C CXX)

####################################################################################################
# Futures 
####################################################################################################

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_SUPPRESS_REGENERATION true)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type" FORCE)
endif()

if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "Create a build directory and run CMake from there.")
endif()

####################################################################################################
# Options
####################################################################################################
option(TESTS            "enable tests"              OFF)

set(SANITIZER "" CACHE STRING "Enable sanitizers (address, undefined, etc.)")

if(NOT "${SANITIZER}" STREQUAL "")
    message(STATUS "SANITIZER: ${SANITIZER}")
    if(MSVC)
        add_compile_options(/fsanitize=${SANITIZER})
        add_link_options(/INFERASANLIBS)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        add_compile_options(-fsanitize=${SANITIZER})
        add_link_options(-fsanitize=${SANITIZER})
    endif()
endif()

####################################################################################################
# WIN32 defines 
####################################################################################################

if(WIN32)
    add_definitions(-DNOMINMAX)
    #shut up about unsafe stuff
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    set(CMAKE_USE_RELATIVE_PATHS true)

else()
    # message(FATAL_ERROR "This project only support Windows. Sorry :)")
endif()

####################################################################################################
# Compiler Check
####################################################################################################

message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} (${CMAKE_CXX_COMPILER_VERSION})")

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "13")
    message(FATAL_ERROR "Compiler version old for this project. Please upgrade your compiler to version 13 or higher.")
  endif()
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "16.10")
    message(FATAL_ERROR "MSVC version old for this project. Please upgrade to Visual Studio 2019 or later.")
  endif()
endif()

####################################################################################################
# Check if 3party exist to not fetch Debs
####################################################################################################

if(EXISTS ${CMAKE_SOURCE_DIR}/3party)
    set(ONLINE FALSE)
else()
    set(ONLINE TRUE)
endif()

include(helpers)
if(ONLINE)

# Include get_cpm.cmake
include(get_cpm)
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

CPMAddPackage(
    NAME glcorearb
    URL https://registry.khronos.org/OpenGL/api/GL/glcorearb.h
    DOWNLOAD_ONLY YES
    DOWNLOAD_NO_EXTRACT TRUE
    DOWNLOAD_NAME glcorearb.h
)

CPMAddPackage(
    NAME khr
    URL https://raw.githubusercontent.com/KhronosGroup/EGL-Registry/main/api/KHR/khrplatform.h
    DOWNLOAD_ONLY YES
    DOWNLOAD_NO_EXTRACT TRUE
    DOWNLOAD_NAME khrplatform.h
)
if(NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/KHR/khrplatform.h)
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/KHR)

    file(RENAME 
        ${CMAKE_CURRENT_BINARY_DIR}/_deps/khr-src/khrplatform.h 
        ${CMAKE_CURRENT_BINARY_DIR}/KHR/khrplatform.h
    )
endif()
include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}/_deps/glcorearb-src  # For glcorearb.h
    ${CMAKE_CURRENT_BINARY_DIR}                      # For KHR/khrplatform.h
)
else()
add_subdirectory(3party/glm)
endif()

####################################################################################################
# Source Files 
####################################################################################################

set(SOURCES
    src/Keyboard.cpp
    src/Mouse.cpp
	src/Window.cpp
	src/Renderer.cpp
	src/Shader.cpp
	src/Transform.cpp
    src/Camera.cpp
    src/Mesh.cpp
    src/OpenGL.cpp
    src/APP.cpp
    src/GameObject.cpp
    src/Texture.cpp
    src/Scene.cpp
    src/Material.cpp
    src/Log.cpp
    src/ResourceManager.cpp
	)


####################################################################################################
# Find instaled packages 
####################################################################################################

# Find OpenGL package
find_package(OpenGL REQUIRED)

# Extract the core OpenGL library (first entry in the list)
list(GET OPENGL_LIBRARIES 0 CORE_OPENGL_LIB)

####################################################################################################
# Build Exe 
####################################################################################################
add_library(core ${SOURCES})
add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE core)
####################################################################################################
# Include Dirs 
####################################################################################################

target_include_directories(core PUBLIC "${CMAKE_SOURCE_DIR}/include")

if(ONLINE)
    set(STB_IMG_HEADER ${stb_image_header_SOURCE_DIR})
else()
    set(STB_IMG_HEADER ${CMAKE_SOURCE_DIR}/3party)
    target_include_directories(core PUBLIC "${CMAKE_SOURCE_DIR}/3party")
endif()

add_library(stb_image INTERFACE)
target_include_directories(stb_image SYSTEM INTERFACE ${STB_IMG_HEADER})

####################################################################################################
# Compile Flags 
####################################################################################################

apply_compile_options(TARGETS core ${PROJECT_NAME})

####################################################################################################
# Link Libs 
####################################################################################################

if(NOT WIN32)
target_link_libraries(core PUBLIC $<$<CXX_COMPILER_ID:Clang>:stdc++exp>)
endif()

target_link_libraries(core
    PUBLIC
        glm::glm
        OpenGL::GL
        stb_image
        $<$<STREQUAL:$<PLATFORM_ID>,Windows>:dbghelp>
        $<$<CXX_COMPILER_ID:GNU>:stdc++exp>
)

if(UNIX)
set(ORIGINAL_SUFFIXES ${CMAKE_FIND_LIBRARY_SUFFIXES})
set(CMAKE_FIND_LIBRARY_SUFFIXES .a ${CMAKE_FIND_LIBRARY_SUFFIXES})
set(CMAKE_FIND_LIBRARY_PREFERS_STATIC ON)

find_package(X11 REQUIRED)

message(STATUS "X11 Lib: ${X11_LIBRARIES}")
message(STATUS "X11 include: ${X11_INCLUDE_DIR}")
set(CMAKE_FIND_LIBRARY_SUFFIXES ${ORIGINAL_SUFFIXES})

target_include_directories(core PUBLIC ${X11_INCLUDE_DIR})
target_link_libraries(core PUBLIC -Wl,-Bstatic -Wl,--start-group ${X11_STATIC_LIBS} -Wl,--end-group -Wl,-Bdynamic )
endif()

target_precompile_headers(core PUBLIC ${CMAKE_SOURCE_DIR}/include/pch.h)

add_compile_definitions(
    PROJECT_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
    OPENGL_LIB="${CORE_OPENGL_LIB}"
    $<$<BOOL:${HAS_STD_STACKTRACE}>:HAS_STD_STACKTRACE>
    $<$<CONFIG:Debug>:DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
)


####################################################################################################
# TEST
####################################################################################################
if(TESTS)
    add_definitions(-DTESTING)
    enable_testing()
    include(CTest)
    add_subdirectory(tests)
endif()

print_target_compile_options(core)
print_target_compile_options(${PROJECT_NAME})