include(CMakeDependentOption)

option(TESTS                "Tests"                                   OFF)
option(DOCS                 "Docs"                                    OFF)
option(ENABLE_COVERAGE      "Code coverage"                           OFF)
option(HARDEN               "Hardening"                               OFF)
option(ASAN                 "Address Sanitizer"                       OFF)
option(USAN                 "Undefined Sanitizer"                     OFF)
option(TSAN                 "Thread Sanitizer"                        OFF)
option(NATIVE_OPT           "Native opt build"                        ON)
option(GAME_HOT_RELOADABLE  "Build Game as Shared and hot reload it"  OFF)
cmake_dependent_option(ROBUST_GL_CHECK "Robust GL checks in Debug builds `slow` " ON [[ CMAKE_BUILD_TYPE STREQUAL "Debug" ]] OFF)
cmake_dependent_option(NO_CONSOLE "No Console with Window" ON [[ CMAKE_BUILD_TYPE STREQUAL "Release" ]] OFF)
cmake_dependent_option(STATIC_LINK "Static link C++" ON "MINGW" OFF)
cmake_dependent_option(RTTI "RunTime Type Info" ON "NOT MINGW" OFF) # disable RTTI becaus of bug in mingw redifinition when static link libstdc++



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
    find_package(Doxygen QUIET)
    set_package_properties(Doxygen PROPERTIES
        TYPE OPTIONAL
        PURPOSE "Used to generate developer documentation."
    )

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
# Print Option Status
####################################################################################################

add_feature_info(Testing             TESTS                "[Build the testing suite]")
add_feature_info(Documentation       DOCS                 "[Build documentation]")
add_feature_info("Code Coverage"     ENABLE_COVERAGE      "[Generate coverage reports]")
add_feature_info(Hardening           HARDEN               "[Security hardening flags]")
add_feature_info("Static Link"       STATIC_LINK          "[Link libraries statically]")
add_feature_info(ASAN                ASAN                 "[Address Sanitizer]")
add_feature_info(USAN                USAN                 "[Undefined Sanitizer]")
add_feature_info("Native Opt"        NATIVE_OPT           "[Native optimization build]")
add_feature_info("Hot Reloading"     GAME_HOT_RELOADABLE  "[Game hot reload support]")
add_feature_info("Robust GL"         ROBUST_GL_CHECK      "[Robust GL checks in Debug]")
add_feature_info("No Console"        NO_CONSOLE           "[No Console with Window]")
add_feature_info(RTTI                RTTI                 "[Run-time type information]")


feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
