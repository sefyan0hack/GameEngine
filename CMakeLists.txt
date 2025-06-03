cmake_minimum_required(VERSION 3.28)

project(Engine LANGUAGES C CXX)

####################################################################################################
# Futures 
####################################################################################################
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

include(CTest)
include(helpers)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_SUPPRESS_REGENERATION true)


if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type" FORCE)
endif()

if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "Create a build directory and run CMake from there.")
endif()
if(WIN32)
    set(CMAKE_USE_WINHTTP_FOR_HTTP ON CACHE BOOL "" FORCE)
    set(CMAKE_USE_CURL OFF CACHE BOOL "" FORCE)
endif()

string(REGEX MATCH "^[0-9]+" COMPILER_MAJOR_VERSION "${CMAKE_CXX_COMPILER_VERSION}")

####################################################################################################
# Options
####################################################################################################
option(TESTS            "enable tests"              OFF)
option(ENABLE_COVERAGE  "enable code coverage"      OFF)
option(HARDEN           "enable Hardening"          OFF)
option(STATIC_LINK      "enable Static link c++"    ON)
if(MINGW) #desable RTTI becaus of bug in mingw redifinition when static link libstdc++
option(RTTI             "enable RunTime Type Info"  OFF)
else()
option(RTTI             "enable RunTime Type Info"  ON)
endif(MINGW)

if(TESTS)
    message(STATUS "[Testing `ON`]")
endif()
if(ENABLE_COVERAGE)
    message(STATUS "[Coverage `ON`]")
endif()
if(HARDEN)
    message(STATUS "[Hardening `ON`]")
endif()
if(STATIC_LINK)
    message(STATUS "[Static Link `ON`]")
endif()
if(RTTI)
    message(STATUS "[Rtti `ON`]")
endif()

set(SANITIZER "" CACHE STRING "Enable sanitizers (address, undefined, etc.)")

####################################################################################################
# 
####################################################################################################
if(NOT "${SANITIZER}" STREQUAL "")
    message(STATUS "SANITIZER: ${SANITIZER}")
    if(MSVC)
        if("${SANITIZER}" MATCHES ".*address.*")
            add_compile_options(/fsanitize=address)
        endif()
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        add_compile_options(-fsanitize=${SANITIZER})
        add_link_options(-fsanitize=${SANITIZER})
    endif()
endif()

if(TESTS AND ENABLE_COVERAGE)
    
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_link_options(--coverage)

        find_program(GCOVR_PATH gcovr REQUIRED)
        find_program(GCOV_PATH NAMES "gcov-${COMPILER_MAJOR_VERSION}" "gcov" REQUIRED)

        if(GCOVR_PATH)
            set(COMMEN_GCOVR_FLAGS
                --root ${CMAKE_SOURCE_DIR}
                --exclude '${CMAKE_SOURCE_DIR}/tests/'
                --exclude '${CMAKE_SOURCE_DIR}/3party/'
                --gcov-executable ${GCOV_PATH}
                --delete
                --exclude-function-lines
                --exclude-unreachable-branches
                --exclude-noncode-lines
                --sort uncovered-percent
                --sort uncovered-number
                --sort-reverse
                --decisions
                --calls
                -s
            )
            add_custom_target(pre_coverage
                COMMAND ${GCOVR_PATH} --version
                COMMAND ${GCOV_PATH} --version
                COMMAND $<TARGET_FILE:unit_tests>
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                DEPENDS core unit_tests
            )
            add_custom_target(coverage
                COMMAND ${GCOVR_PATH} ${COMMEN_GCOVR_FLAGS} --xml -o coverage
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                DEPENDS pre_coverage
                COMMENT "Generating GCC xml coverage report with gcovr..."
            )

            add_custom_target(coverage_html
                COMMAND ${GCOVR_PATH} ${COMMEN_GCOVR_FLAGS} --html-details -o coverage.html
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                DEPENDS pre_coverage
                COMMENT "Generating GCC html coverage report with gcovr..."
            )
        endif()
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        
        add_link_options(-fprofile-instr-generate)
        
        # find_program(LCOV_PATH lcov REQUIRED)
        find_program(LLVM_PROFDATA_PATH llvm-profdata REQUIRED)
        find_program(LLVM_COV_PATH llvm-cov REQUIRED)

        set(ENV{LLVM_PROFILE_FILE} "default.profraw")

        add_custom_target(pre_coverage
            COMMAND ${LLVM_COV_PATH} --version
            COMMAND ${CMAKE_COMMAND} -E remove "default.profraw"
            COMMAND ${CMAKE_COMMAND} -E remove "coverage.profdata"
            COMMAND $<TARGET_FILE:unit_tests>
            COMMAND ${LLVM_PROFDATA_PATH} merge -sparse default.profraw -o coverage.profdata
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            DEPENDS core unit_tests
        )

        add_custom_target(coverage
            COMMAND ${LLVM_COV_PATH} export $<TARGET_FILE:unit_tests>
                -instr-profile=coverage.profdata
                -format=lcov
                -ignore-filename-regex='${CMAKE_SOURCE_DIR}/tests/.*'
                -ignore-filename-regex='${CMAKE_SOURCE_DIR}/3party/.*'
                > ${CMAKE_BINARY_DIR}/coverage
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            DEPENDS pre_coverage
            COMMENT "Generating lcov coverage report for Codecov.io"
        )

        add_custom_target(coverage_html
            COMMAND ${LLVM_COV_PATH} show $<TARGET_FILE:unit_tests>
                -instr-profile=coverage.profdata
                -output-dir=${CMAKE_BINARY_DIR}/coverage_html
                -format=html
                -ignore-filename-regex='${CMAKE_SOURCE_DIR}/tests/.*'
                -ignore-filename-regex='${CMAKE_SOURCE_DIR}/3party/.*'
                --show-mcdc-summary
                --show-mcdc
                -show-instantiation-summary
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            DEPENDS pre_coverage
            COMMENT "Generating Clang html coverage reports..."
        )

    endif()

endif()

if(STATIC_LINK)
    if(MSVC)
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        if(NOT WIN32)
            add_link_options(-static-libgcc -static-libstdc++)
        endif()
    endif()
endif()


rtti(${RTTI})

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
message(STATUS "=== Detailed Host Information ===")
message(STATUS "Platform: ${CMAKE_SYSTEM_NAME} (${CMAKE_SYSTEM_VERSION}), Compiler: ${CMAKE_CXX_COMPILER_ID} (${CMAKE_CXX_COMPILER_VERSION}) , Arch: (${CMAKE_SYSTEM_PROCESSOR})")
set(QUERIES
    NUMBER_OF_LOGICAL_CORES
    NUMBER_OF_PHYSICAL_CORES
    TOTAL_VIRTUAL_MEMORY
    AVAILABLE_VIRTUAL_MEMORY
    TOTAL_PHYSICAL_MEMORY
    AVAILABLE_PHYSICAL_MEMORY
    HOSTNAME
    FQDN
    OS_NAME
    OS_RELEASE
    OS_VERSION
    OS_PLATFORM
)

foreach(query IN LISTS QUERIES)
    cmake_host_system_information(RESULT result QUERY ${query})
    message(STATUS "${query}: ${result}")
endforeach()

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

# Find Threads package
find_package(Threads REQUIRED)

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

apply_all_options(TARGETS core ${PROJECT_NAME})
apply_coverage_options(TARGETS core ${PROJECT_NAME})
apply_harden_options(TARGETS core)
apply_sanitizer_options(TARGETS core)

####################################################################################################
# Link Libs 
####################################################################################################

if(NOT WIN32)
target_link_libraries(core PRIVATE $<$<CXX_COMPILER_ID:Clang>:stdc++exp>)
endif()

target_link_libraries(core
    PUBLIC
        glm::glm
    PRIVATE
        stb_image
        ${OPENGL_LIBRARIES}
        Threads::Threads
        $<$<STREQUAL:$<PLATFORM_ID>,Windows>:dbghelp>
        $<$<STREQUAL:$<PLATFORM_ID>,Linux>:dl;${X11_LIBRARIES}>
        $<$<CXX_COMPILER_ID:GNU>:stdc++exp>
)



target_precompile_headers(core PUBLIC ${CMAKE_SOURCE_DIR}/include/pch.h)

add_compile_definitions(
    PROJECT_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
    OPENGL_LIB="${CORE_OPENGL_LIB}"
    PLATFORM_NAME="${CMAKE_SYSTEM_NAME}"
    PLATFORM_VERSION="${CMAKE_SYSTEM_VERSION}"
    PLATFORM_ARCH="${CMAKE_SYSTEM_PROCESSOR}"

    $<$<CONFIG:Debug>:DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
)


####################################################################################################
# TEST
####################################################################################################
if(TESTS)
    message(STATUS "[Tests `ON`]")
    enable_testing()
    add_definitions(-DTESTING)
    add_subdirectory(tests/unit)
endif()

print_target_compile_options(core)
print_target_compile_options(${PROJECT_NAME})