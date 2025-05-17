cmake_minimum_required(VERSION 3.28)

project(Engine LANGUAGES C CXX)

####################################################################################################
# Futures 
####################################################################################################

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_SUPPRESS_REGENERATION true)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)

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
option(CODECOVERAGE     "enable code coverage"     OFF)

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

if(CODECOVERAGE)
        add_compile_options($<$<CXX_COMPILER_ID:GNU,Clang>:--coverage>)
        add_link_options($<$<CXX_COMPILER_ID:GNU,Clang>:--coverage>)
        link_libraries($<$<CXX_COMPILER_ID:GNU>:gcov>)
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
# Find X11 package
if(UNIX)
find_package(X11 REQUIRED)
endif()

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

target_include_directories(core PUBLIC 
    "${CMAKE_SOURCE_DIR}/include"
    $<$<STREQUAL:$<PLATFORM_ID>,Linux>:${X11_INCLUDE_DIR}>
)

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
        stb_image
        ${OPENGL_LIBRARIES}
        $<$<STREQUAL:$<PLATFORM_ID>,Windows>:dbghelp>
        $<$<STREQUAL:$<PLATFORM_ID>,Linux>:dl;${X11_LIBRARIES}>
        $<$<CXX_COMPILER_ID:GNU>:stdc++exp>
)
set_target_properties(core PROPERTIES CXX_VISIBILITY_PRESET hidden)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_VISIBILITY_PRESET hidden)

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
    message(STATUS "[Tests `ON`]")

    add_definitions(-DTESTING)
    enable_testing()
    include(CTest)
    add_subdirectory(tests)

    if(CODECOVERAGE)
        message(STATUS "[Code Coverage `ON`]")

        if(WIN32 AND MSVC)
            find_program(OPENCPPCOVERAGE_PATH OpenCppCoverage.exe)
            if(OPENCPPCOVERAGE_PATH)
                file(TO_NATIVE_PATH "${CMAKE_SOURCE_DIR}/src" NATIVE_SOURCE_DIR)
                file(TO_NATIVE_PATH "${CMAKE_BINARY_DIR}/coverage_report" NATIVE_OUTPUT_DIR)
                file(TO_NATIVE_PATH "${CMAKE_SOURCE_DIR}/3party" NATIVE_3PARTY_PATTERN)
                file(TO_NATIVE_PATH "$<TARGET_FILE:tests>" NATIVE_TESTS)
                    
                set(EXCLUDED_SOURCES
                    --excluded_sources "${NATIVE_3PARTY_PATTERN}\\*"
                    --excluded_sources "*.h"
                    --excluded_sources "*.hpp"
                    --excluded_sources "*.inl"
                    --excluded_sources ".*\\\\predefined C\\+\\+ types.*"
                    --excluded_sources ".*\\\\Microsoft Visual Studio.*"
                    --excluded_modules ".*msvcp.*.dll"
                    --excluded_modules ".*vcruntime.*.dll"
                )
                add_custom_target(coverage
                COMMAND ${CMAKE_COMMAND} -E make_directory "${NATIVE_OUTPUT_DIR}"
                COMMAND ${OPENCPPCOVERAGE_PATH}
                    --quiet
                    --sources ${NATIVE_SOURCE_DIR}
                    --export_type html:${NATIVE_OUTPUT_DIR}
                    ${EXCLUDED_SOURCES}
                    -- ${NATIVE_TESTS}
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                COMMENT "Running OpenCppCoverage..."
                DEPENDS core
                )
            endif()
            else()
                find_program(GCOVR_PATH gcovr)

                if(GCOVR_PATH)

                set(GCOVR_ADDITIONAL_ARGS 
                    "--calls"
                    "--branches"
                    "--decisions"
                    "--exclude-function-lines"
                    "--exclude-noncode-lines"
                    "--exclude-unreachable-branches"
                    "--exclude-throw-branches"
                    "--delete"
                )

                set(COVERAGE_EXCLUDES
                    "${CMAKE_SOURCE_DIR}/3party/.*"
                    "${CMAKE_SOURCE_DIR}/tests/.*" 
                    ".*/CMakeFiles/.*"             
                    ".*cmake_pch.*"                
                )
                set(GCOVR_EXCLUDE_ARGS "")
                foreach(EXCLUDE ${COVERAGE_EXCLUDES})
                    list(APPEND GCOVR_EXCLUDE_ARGS "--exclude=${EXCLUDE}")
                endforeach()
                
                set(GCOVR_COMMON_ARGS
                    --verbose
                    --root=${CMAKE_SOURCE_DIR}
                    --object-directory=${CMAKE_BINARY_DIR}
                    ${GCOVR_EXCLUDE_ARGS}
                    ${GCOVR_ADDITIONAL_ARGS}
                )

                if(UNIX)
                    set(GCOV_BIN "gcov-14")
                    set(GCOVR_ENV_VARS
                        COMMAND ${CMAKE_COMMAND} -E env "GCOV=${GCOV_BIN} -o ${CMAKE_BINARY_DIR}"
                    )
                    list(APPEND GCOVR_COMMON_ARGS --gcov-executable ${GCOV_BIN})
                endif()
                
                # XML Target
                add_custom_target(coverage_xml
                    COMMAND ${CMAKE_CTEST_COMMAND} --test-dir ${PROJECT_BINARY_DIR} --output-on-failure
                    COMMAND ${GCOVR_PATH}
                    --xml
                    --output=${PROJECT_BINARY_DIR}/coverage.xml
                    ${GCOVR_COMMON_ARGS}

                    BYPRODUCTS coverage.xml
                    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
                    DEPENDS core tests
                    COMMENT "Generating XML coverage report "

                    VERBATIM
                )
                # HTML Target
                add_custom_target(coverage_html
                    COMMAND ${CMAKE_CTEST_COMMAND} --test-dir ${PROJECT_BINARY_DIR} --output-on-failure
                    COMMAND ${GCOVR_PATH}
                        --html
                        --html-details
                        --output=${PROJECT_BINARY_DIR}/coverage.html
                        ${GCOVR_COMMON_ARGS}
                    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
                    BYPRODUCTS coverage.html
                    DEPENDS core tests
                    COMMENT "Generating HTML coverage report "
                    VERBATIM
                )
            endif()
        endif()
    endif()
endif()

print_target_compile_options(core)
print_target_compile_options(${PROJECT_NAME})