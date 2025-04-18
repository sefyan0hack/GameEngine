cmake_minimum_required(VERSION 3.28)

project(Engine LANGUAGES C CXX)

if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "Create a build directory and run CMake from there.")
endif()
####################################################################################################
# Futures 
####################################################################################################

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_SUPPRESS_REGENERATION true)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

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
  if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "14.2")
    message(FATAL_ERROR "Compiler version old for this project. Please upgrade your compiler to version 14.2 or higher.")
  endif()
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "19.40")
    message(FATAL_ERROR "MSVC version old for this project. Please upgrade to Visual Studio 2022 or later.")
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
    src/main.cpp
    src/Keyboard.cpp
    src/Mouse.cpp
	src/Window.cpp
	src/XWindow.cpp
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
    src/Log.cpp
    src/ResourceManager.cpp
	)


####################################################################################################
# Find instaled packages 
####################################################################################################

# Find OpenGL package
find_package(OpenGL REQUIRED)

####################################################################################################
# Build Exe 
####################################################################################################

add_executable(${PROJECT_NAME} ${SOURCES})

####################################################################################################
# Include Dirs 
####################################################################################################

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/include")

if(ONLINE)
    set(STB_IMG_HEADER ${stb_image_header_SOURCE_DIR})
else()
    set(STB_IMG_HEADER ${CMAKE_SOURCE_DIR}/3party)
endif()

add_library(stb_image INTERFACE)
target_include_directories(stb_image SYSTEM INTERFACE ${STB_IMG_HEADER})

####################################################################################################
# Compile Flags 
####################################################################################################

apply_compile_options(TARGETS ${PROJECT_NAME})

print_target_compile_options(${PROJECT_NAME})

target_compile_definitions(${PROJECT_NAME}
    PRIVATE
        PROJECT_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
        $<$<CONFIG:Debug>:DEBUG>
)

####################################################################################################
# Link Libs 
####################################################################################################
if(UNIX)
find_package(X11 REQUIRED)
target_include_directories(${PROJECT_NAME} PRIVATE ${X11_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE ${X11_LIBRARIES})
endif()

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        glm::glm
        OpenGL::GL
        stb_image
        $<$<STREQUAL:$<PLATFORM_ID>,Windows>:dbghelp>
        $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:stdc++exp>
)
target_precompile_headers(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/include/pch.h)

####################################################################################################
# Separate debug symbols from release buld (GNU or Clang)
####################################################################################################

if(FALSE AND CMAKE_BUILD_TYPE STREQUAL "Release")
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} --only-keep-debug 
        $<TARGET_FILE:${PROJECT_NAME}>
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/$<TARGET_FILE_BASE_NAME:${PROJECT_NAME}>.debug
        COMMAND ${CMAKE_OBJCOPY} --strip-debug $<TARGET_FILE:${PROJECT_NAME}>
        COMMAND ${CMAKE_OBJCOPY} 
        --add-gnu-debuglink=$<TARGET_FILE_DIR:${PROJECT_NAME}>/$<TARGET_FILE_BASE_NAME:${PROJECT_NAME}>.debug 
        $<TARGET_FILE:${PROJECT_NAME}>
        COMMENT "Separating debug symbols into $<TARGET_FILE_BASE_NAME:${PROJECT_NAME}>.debug"
    )
    endif()
endif()

####################################################################################################
# Code Coverage
####################################################################################################
include(CTest)
enable_testing()

find_program(GCOVR_EXECUTABLE gcovr)
if(GCOVR_EXECUTABLE)
  add_custom_target(coverage
    COMMENT "Generating coverage report"
    COMMAND ${GCOVR_EXECUTABLE}
      -r ${CMAKE_SOURCE_DIR}
      --html --html-details
      --exclude ".*/3party/.*"
      --exclude ".*\\.h"
      --exclude ".*\\.hpp"
      --exclude ".*\\.inl"
      -o coverage.html
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  )

  add_dependencies(coverage ${PROJECT_NAME})

  if(NOT MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_${_LANG}_FLAGS} -pg -g --coverage -O0 -fprofile-arcs -ftest-coverage")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage -fprofile-arcs -ftest-coverage")
  endif()
endif()