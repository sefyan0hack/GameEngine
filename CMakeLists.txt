cmake_minimum_required(VERSION 3.28)

project(Engine LANGUAGES C CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

include(CTest)
include(helpers)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_USE_RELATIVE_PATHS true)

if(NOT CMAKE_BUILD_TYPE)
set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type" FORCE)
endif()

string(REGEX MATCH "^[0-9]+" COMPILER_MAJOR_VERSION "${CMAKE_CXX_COMPILER_VERSION}")

####################################################################################################
# Options
####################################################################################################
option(TESTS            "enable tests"               OFF)
option(ENABLE_COVERAGE  "enable code coverage"       OFF)
option(HARDEN           "enable Hardening"           OFF)
option(STATIC_LINK      "enable Static link C++"     ON)
option(ASAN             "enable Address Sanitizer"   OFF)
option(USAN             "enable Undefined Sanitizer" OFF)
option(TSAN             "enable Thread Sanitizer"    OFF)
option(EXCEPTIONS       "enable Exciptions"          OFF)

if(MINGW) #desable RTTI becaus of bug in mingw redifinition when static link libstdc++
option(RTTI             "enable RunTime Type Info"   OFF)
else()
option(RTTI             "enable RunTime Type Info"   ON)
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
if(ASAN)
    message(STATUS "[Address Sanitizer `ON`]")
endif()
if(USAN)
    message(STATUS "[Undefined Sanitizer `ON`]")
endif()
if(TSAN)
    message(STATUS "[Thread Sanitizer `ON`]")
endif()
if(RTTI)
    message(STATUS "[Rtti `ON`]")
endif()
if(EXCEPTIONS)
    message(STATUS "[Exceptions `ON`]")
endif()

####################################################################################################
# Emscripten-specific setup 
####################################################################################################

if(EMSCRIPTEN)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
    set(STATIC_LINK OFF)
    set(RTTI OFF)
    
    add_compile_options(
        -Oz
        -g0
        -std=c++23
        -Wno-character-conversion -Wno-dollar-in-identifier-extension
        -pthread
        -msimd128
        -fPIC
        -fno-exceptions -fno-rtti
    )

    add_link_options(
        -Oz
        -g0
        -sASYNCIFY
        -sUSE_WEBGL2=1
        -sMIN_WEBGL_VERSION=1
        -sMAX_WEBGL_VERSION=2
        -sALLOW_MEMORY_GROWTH=1
        -sNO_EXIT_RUNTIME=1
        -sFULL_ES3=1
        -sSTB_IMAGE=1
        -sDISABLE_EXCEPTION_THROWING=1
        -sWASM_LEGACY_EXCEPTIONS=0
        -sELIMINATE_DUPLICATE_FUNCTIONS=1
        -fno-exceptions -fno-rtti
        --embed-file ${CMAKE_SOURCE_DIR}/res
        --llvm-lto 1
    )

    configure_file(
        ${CMAKE_SOURCE_DIR}/cmake/index_template.cmake.in
        ${CMAKE_BINARY_DIR}/index_html.cmake
        @ONLY
    )

    add_custom_command(
        OUTPUT ${CMAKE_BINARY_DIR}/index.html
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/index_html.cmake ${CMAKE_BINARY_DIR}/index.html
        DEPENDS ${CMAKE_BINARY_DIR}/index_html.cmake ${PROJECT_NAME}
        COMMENT "Generating index.html"
    )
    
    add_custom_target(generate_index_html ALL
        DEPENDS ${CMAKE_BINARY_DIR}/index.html
    )
endif()
####################################################################################################
# 
####################################################################################################

if(TESTS AND ENABLE_COVERAGE)
    
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_link_options(--coverage)

        find_program(GCOVR_PATH gcovr REQUIRED)
        find_program(GCOV_PATH NAMES "gcov-${COMPILER_MAJOR_VERSION}" "gcov" REQUIRED)

        if(GCOVR_PATH)
            set(COMMEN_GCOVR_FLAGS
                --root ${CMAKE_SOURCE_DIR}
                --exclude '${CMAKE_SOURCE_DIR}/tests/.*'
                --exclude '${CMAKE_SOURCE_DIR}/3party/.*'
                --exclude '${CMAKE_BINARY_DIR}/.*'
                --exclude '/usr/.*'
                --exclude '[A-Z]:/msys64/.*'
                --exclude '[A-Z]:/mingw64/.*'
                --exclude '.*/include/c++/.*'
                --gcov-executable ${GCOV_PATH}
                --delete
                --exclude-function-lines
                --exclude-unreachable-branches
                --exclude-throw-branches
                --exclude-noncode-lines
                --sort=uncovered-percent
                --merge-mode-functions=merge-use-line-min
                --sort-reverse
                --decisions
                --calls
                -s
            )
            add_custom_target(pre_coverage
                COMMAND ${GCOVR_PATH} --version
                COMMAND ${GCOV_PATH} --version
                COMMAND ${CMAKE_COMMAND} -E env "TESTING_ENABLED=1"  $<TARGET_FILE:unit_tests>
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


        add_custom_target(pre_coverage
            COMMAND ${LLVM_COV_PATH} --version
            COMMAND ${CMAKE_COMMAND} -E remove "default.profraw"
            COMMAND ${CMAKE_COMMAND} -E remove "coverage.profdata"
            COMMAND ${CMAKE_COMMAND} -E env "LLVM_PROFILE_FILE=default.profraw" "TESTING_ENABLED=1" $<TARGET_FILE:unit_tests>
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
                -Xdemangler=c++filt -Xdemangler=-n
                -show-line-counts-or-regions
                -show-instantiations
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

if(EXCEPTIONS)
    add_compile_options( 
        "$<$<CXX_COMPILER_ID:MSVC>:/EHsc>"
        "$<$<CXX_COMPILER_ID:Clang,GNU>:-fexceptions>"
    )
else()
    if(MSVC)
    add_definitions(/D_HAS_EXCEPTIONS=0)
    string(REGEX REPLACE "/EH[a-z]+" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    endif()
    add_compile_options(
        "$<$<CXX_COMPILER_ID:MSVC>:/EHs-c-;/wd4530>"
        "$<$<CXX_COMPILER_ID:Clang,GNU>:-fno-exceptions>"
    )
endif()

# Check whether the compiler supports an interprocedural optimization (IPO/LTO)
include(CheckIPOSupported)
check_ipo_supported(RESULT LTO_SUPPORTED)

if(LTO_SUPPORTED)
  set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)  # Enable LTO globally
else()
  message(WARNING "LTO not supported by cross-compiler!")
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
# Check if We Want Fetch or include Deps
####################################################################################################

if(EXISTS ${CMAKE_SOURCE_DIR}/3party)
    set(ONLINE FALSE)
else()
    set(ONLINE TRUE)
endif()

if(ONLINE)
    include(FetchDeps)
else()
    include(IncludeDeps)
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

if(NOT EMSCRIPTEN)

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

endif()
####################################################################################################
# Build Exe 
####################################################################################################
add_library(core ${SOURCES})
add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm core)
####################################################################################################
# Include Dirs 
####################################################################################################

target_include_directories(core PUBLIC
    "${CMAKE_SOURCE_DIR}/include"
    $<$<PLATFORM_ID:Linux>:${X11_INCLUDE_DIR}>
)

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
# common Libs
target_link_libraries(core
    PRIVATE
        glm::glm
        stb_image
)

# common (Not Web)
if(NOT EMSCRIPTEN)
    if(NOT WIN32)
    target_link_libraries(core PRIVATE $<$<CXX_COMPILER_ID:Clang>:stdc++exp>)
    endif()

    target_link_libraries(core
        PRIVATE
            Threads::Threads
            ${OPENGL_LIBRARIES}
            $<$<STREQUAL:$<PLATFORM_ID>,Windows>:dbghelp>
            $<$<CXX_COMPILER_ID:GNU>:stdc++exp>
            $<$<PLATFORM_ID:Linux>:dl>
            $<$<PLATFORM_ID:Linux>:${X11_LIBRARIES}>
    )
endif()

target_precompile_headers(core PUBLIC ${CMAKE_SOURCE_DIR}/include/pch.h)

add_compile_definitions(
    PROJECT_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
    OPENGL_LIB="${CORE_OPENGL_LIB}"
    PLATFORM_NAME="${CMAKE_SYSTEM_NAME}"
    PLATFORM_VERSION="${CMAKE_SYSTEM_VERSION}"
    PLATFORM_ARCH="${CMAKE_SYSTEM_PROCESSOR}"
    
    $<$<PLATFORM_ID:Windows>:WINDOWS_PLT>
    $<$<PLATFORM_ID:Linux>:LINUX_PLT>
    $<$<PLATFORM_ID:Emscripten>:WEB_PLT>

    $<$<CXX_COMPILER_ID:GNU>:GNU_CPL>
    $<$<CXX_COMPILER_ID:Clang>:CLANG_CPL>
    $<$<CXX_COMPILER_ID:MSVC>:MSVC_CPL>

    $<$<CONFIG:Debug>:DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
)

####################################################################################################
# TEST
####################################################################################################
if(TESTS)
    message(STATUS "[Tests `ON`]")
    enable_testing()
    add_subdirectory(tests/unit)
    
    set_target_properties(unit_tests PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION FALSE
    )
endif()

remove_global_flags(Engine)
remove_global_flags(core)

print_target_compile_options(core)
print_target_compile_options(${PROJECT_NAME})