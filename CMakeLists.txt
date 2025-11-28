cmake_minimum_required(VERSION 3.28)

project(
    GameEngine 
    LANGUAGES C CXX 
    VERSION 0.0.1
)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

set(DIR_PLATFORM_NAME

    "$<$<PLATFORM_ID:Windows>:windows>"
    "$<$<PLATFORM_ID:Linux>:linux>"
    "$<$<PLATFORM_ID:Emscripten>:web>"
)

include(CTest)
include(helpers)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_USE_RELATIVE_PATHS true)

if(WIN32)
    add_compile_definitions(
        _CRT_SECURE_NO_WARNINGS
        WIN32_LEAN_AND_MEAN
        NOMINMAX
    )
endif()

string(REGEX MATCH "^[0-9]+" COMPILER_MAJOR_VERSION "${CMAKE_CXX_COMPILER_VERSION}")

####################################################################################################
# Options
####################################################################################################
include(CMakeDependentOption)

option(TESTS                "Tests"               OFF)
option(DOCS                 "Docs"                OFF)
option(ENABLE_COVERAGE      "Code coverage"       OFF)
option(HARDEN               "Hardening"           OFF)
option(ASAN                 "Address Sanitizer"   OFF)
option(USAN                 "Undefined Sanitizer" OFF)
option(TSAN                 "Thread Sanitizer"    OFF)
option(NATIVE_OPT           "Native opt build"    ON)
cmake_dependent_option(ROBUST_GL_CHECK "Robust GL checks in Debug builds `slow` " ON [[ CMAKE_BUILD_TYPE STREQUAL "Debug" ]] OFF)
cmake_dependent_option(NO_CONSOLE "No Console with Window" ON [[ CMAKE_BUILD_TYPE STREQUAL "Release" ]] OFF)
cmake_dependent_option(STATIC_LINK "Static link C++" ON "MINGW" OFF)
cmake_dependent_option(RTTI "RunTime Type Info" ON "NOT MINGW" OFF) # disable RTTI becaus of bug in mingw redifinition when static link libstdc++

####################################################################################################
# Vsisibility
####################################################################################################

set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)

####################################################################################################
# Position Independent Code
####################################################################################################

set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)

####################################################################################################
# Link Time OPT
####################################################################################################
include(CheckIPOSupported)
check_ipo_supported(RESULT IPO_SUPPORTED OUTPUT OUTPUT)

if(IPO_SUPPORTED AND NOT TESTS)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

add_compile_options(
    "$<$<CXX_COMPILER_ID:GNU,Clang>:-ffunction-sections;-fdata-sections>"
    "$<$<CXX_COMPILER_ID:GNU,Clang>:-funwind-tables>" # generate some exception data (usually disabled for C programs)
    "$<$<CXX_COMPILER_ID:GNU>:-flto-partition=none>"
    "$<$<CXX_COMPILER_ID:MSVC>:/Gy;/Gw>"
    )
    
    add_link_options(
        "$<$<CXX_COMPILER_ID:GNU,Clang>:-Wl,--gc-sections>"
        "$<$<CXX_COMPILER_ID:GNU>:-flto-partition=none>"
        "$<$<CXX_COMPILER_ID:GNU,Clang>:-fuse-linker-plugin>"
        "$<$<CXX_COMPILER_ID:MSVC>:/OPT:REF;/OPT:ICF>"
)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    # GCC requires gcc-ar/gcc-ranlib to handle LTO objects
    find_program(GCC_AR NAMES "${CMAKE_CXX_COMPILER_PREFIX}gcc-ar" gcc-ar)
    find_program(GCC_RANLIB NAMES "${CMAKE_CXX_COMPILER_PREFIX}gcc-ranlib" gcc-ranlib)
    find_program(GCC_NM NAMES "${CMAKE_CXX_COMPILER_PREFIX}gcc-nm" gcc-nm)

    if(GCC_AR AND GCC_RANLIB)
        set(CMAKE_AR ${GCC_AR})
        set(CMAKE_RANLIB ${GCC_RANLIB})
        set(CMAKE_NM ${GCC_NM})
        set(CMAKE_CXX_COMPILER_AR ${GCC_AR})
        set(CMAKE_CXX_COMPILER_RANLIB ${GCC_RANLIB})
    endif()

elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    # Clang uses LLVM bitcode, which standard 'ar' cannot read
    find_program(LLVM_AR NAMES "${CMAKE_CXX_COMPILER_PREFIX}llvm-ar" llvm-ar)
    find_program(LLVM_RANLIB NAMES "${CMAKE_CXX_COMPILER_PREFIX}llvm-ranlib" llvm-ranlib)
    find_program(LLVM_NM NAMES "${CMAKE_CXX_COMPILER_PREFIX}llvm-nm" llvm-nm)

    if(LLVM_AR AND LLVM_RANLIB)
        set(CMAKE_AR ${LLVM_AR})
        set(CMAKE_RANLIB ${LLVM_RANLIB})
        set(CMAKE_NM ${LLVM_NM})
        set(CMAKE_CXX_COMPILER_AR ${LLVM_AR})
        set(CMAKE_CXX_COMPILER_RANLIB ${LLVM_RANLIB})
    endif()

elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    # MSVC handles LTO via /LTCG and lib.exe automatically.
    # No tool swapping is needed
endif()
####################################################################################################
# Emscripten-specific setup 
####################################################################################################

if(EMSCRIPTEN)
    set(OPENGL_LIB_NAME "WebGL")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
    set(CMAKE_EXECUTABLE_SUFFIX ".html")

    set(STATIC_LINK OFF)
    set(NATIVE_OPT OFF)

    add_compile_options(
        -Oz
        -Wno-character-conversion -Wno-dollar-in-identifier-extension
        -msimd128
        -fPIC
        -pthread
        -matomics
        -flto
        -fexperimental-library
    )

    add_link_options(
        -Oz
        -flto
        -pthread
        -matomics
        -sASYNCIFY
        -sUSE_WEBGL2=1
        -sMIN_WEBGL_VERSION=1
        -sMAX_WEBGL_VERSION=2
        -sALLOW_MEMORY_GROWTH=1 # -pthread + ALLOW_MEMORY_GROWTH may run non-wasm code slowly
        -sFULL_ES3=1
        -sDISABLE_EXCEPTION_THROWING=0
        -sDISABLE_EXCEPTION_CATCHING=0
        -sFILESYSTEM=1 # needed for DynLib 
        -sPTHREAD_POOL_SIZE=4
        -sEXIT_RUNTIME=1
        -sUSE_PTHREADS=1
        -sERROR_ON_UNDEFINED_SYMBOLS=0
        -sASSERTIONS=1
    )

    configure_file(
        ${CMAKE_SOURCE_DIR}/cmake/index_template.cmake.in
        ${CMAKE_BINARY_DIR}/index_html.cmake
        @ONLY
    )

    add_custom_command(
        OUTPUT ${CMAKE_BINARY_DIR}/index.html
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/index_html.cmake ${CMAKE_BINARY_DIR}/index.html
        DEPENDS ${CMAKE_BINARY_DIR}/index_html.cmake Luncher
        COMMENT "Generating index.html"
    )
    
    add_custom_target(generate_index_html ALL
        DEPENDS ${CMAKE_BINARY_DIR}/index.html
    )

    # COOP and COEP headers trick
    file(DOWNLOAD
        "https://raw.githubusercontent.com/gzuidhof/coi-serviceworker/master/coi-serviceworker.js"
        "${CMAKE_BINARY_DIR}/coi-serviceworker.js"
        SHOW_PROGRESS
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
                DEPENDS Engine unit_tests
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
            DEPENDS Engine unit_tests
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
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
            add_link_options(-static-libgcc -static-libstdc++)
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND NOT WIN32)
            add_link_options(-static-libgcc -static-libstdc++)
    endif()
endif()

rtti(${RTTI})


if(DOCS)
    find_package(Doxygen REQUIRED)

    set(DOXY_IN    "${CMAKE_SOURCE_DIR}/Doxyfile.in")
    set(DOXY_OUT   "${CMAKE_BINARY_DIR}/Doxyfile")
   
    configure_file(${DOXY_IN} ${DOXY_OUT} @ONLY)
   
    add_custom_target(docs
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXY_OUT}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM
    )

endif()

if(NATIVE_OPT AND NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    if(NOT MSVC)
        add_compile_options(-march=native)
    else()
        add_compile_options($<IF:$<BOOL:${CMAKE_SYSTEM_PROCESSOR}>,/arch:AVX2,>)
    endif()
endif()

if(ROBUST_GL_CHECK)
    add_compile_definitions( ROBUST_GL_CHECK )
endif()


####################################################################################################
# Compiler Check
####################################################################################################

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
# Resources
####################################################################################################
include(CMakeRC)

file(GLOB_RECURSE RES_FILES
    RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
    CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/res/*"
)

cmrc_add_resource_library( resources
    ALIAS res
    NAMESPACE res

    ${RES_FILES}
)

set_target_properties(resources PROPERTIES POSITION_INDEPENDENT_CODE TRUE)


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


add_compile_definitions(
    PLATFORM_NAME="${CMAKE_SYSTEM_NAME}"
    PLATFORM_VERSION="${CMAKE_SYSTEM_VERSION}"
    PLATFORM_ARCH="${CMAKE_SYSTEM_PROCESSOR}"

    OPENGL_LIB="${CORE_OPENGL_LIB}"
    DYN_LIB_PREFIX="${CMAKE_SHARED_LIBRARY_PREFIX}"
    DYN_LIB_SUFFIX="${CMAKE_SHARED_LIBRARY_SUFFIX}"
    
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
# ??
####################################################################################################
add_subdirectory(Engine)
add_subdirectory(Game)

####################################################################################################
# TEST
####################################################################################################
if(TESTS)
    enable_testing()
    include(GoogleTest)
    add_subdirectory(tests/unit)
    target_link_libraries(unit_tests PUBLIC res)

endif()

####################################################################################################
# Print Option Status
####################################################################################################
message(STATUS "TESTS : (${TESTS}) [ Testing ]")
message(STATUS "DOCS : (${DOCS}) [ Docs ]")
message(STATUS "ENABLE_COVERAGE : (${ENABLE_COVERAGE}) [ Coverage ]")
message(STATUS "HARDEN : (${HARDEN}) [ Hardening ]")
message(STATUS "STATIC_LINK : (${STATIC_LINK}) [ Static Link ]")
message(STATUS "ASAN : (${ASAN}) [ Address Sanitizer ]")
message(STATUS "USAN : (${USAN}) [ Undefined Sanitizer ]")
message(STATUS "TSAN : (${TSAN}) [ Thread Sanitizer ]")
message(STATUS "NATIVE_OPT : (${NATIVE_OPT}) [ Native opt build ]")
message(STATUS "ROBUST_GL_CHECK : (${ROBUST_GL_CHECK}) [ Robust GL checks in Debug builds ]")
message(STATUS "NO_CONSOLE : (${NO_CONSOLE}) [ No Console with Window ]")
message(STATUS "RTTI : (${RTTI}) [ Rtti ]")