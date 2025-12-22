# Engine/cmakelists.txt

include(helpers)
include(GenerateExportHeader)

add_subdirectory(core)
add_subdirectory(graphics)
add_subdirectory(input)

get_target_property(CORE_SRCS core SOURCES)
get_target_property(INPUT_SRCS input SOURCES)
get_target_property(GRAPHICS_SRCS graphics SOURCES)

add_library(Engine)
set_target_properties(Engine PROPERTIES POSITION_INDEPENDENT_CODE TRUE)

generate_export_header(Engine)

target_include_directories(Engine PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

target_precompile_headers(Engine PUBLIC "${CMAKE_SOURCE_DIR}/Engine/Platform.hpp")

if(MSVC)
    target_compile_options(Engine PRIVATE /wd4251 /wd4275)
endif()

target_sources( Engine
    PRIVATE
        ${CORE_SRCS}
        ${INPUT_SRCS}
        ${GRAPHICS_SRCS}
)

if(ANDROID)
    add_library(native_app_glue STATIC ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)
    target_include_directories(native_app_glue PUBLIC ${ANDROID_NDK}/sources/android/native_app_glue)
endif()

target_link_libraries(Engine
    PUBLIC
        core
        graphics
        input
        $<$<PLATFORM_ID:Android>:android>
        $<$<PLATFORM_ID:Android>:EGL>
        $<$<PLATFORM_ID:Android>:GLESv3>
        $<$<PLATFORM_ID:Android>:log>
        $<$<PLATFORM_ID:Android>:native_app_glue>
)

apply_all_options(TARGETS Engine)
apply_coverage_options(TARGETS Engine)
apply_harden_options(TARGETS Engine)
apply_sanitizer_options(TARGETS Engine)

# common (Not Web)
if(NOT EMSCRIPTEN)
    if(LINUX)
        target_link_libraries(Engine PRIVATE $<$<CXX_COMPILER_ID:Clang>:stdc++exp>)
    endif()

    target_link_libraries(Engine
        PRIVATE
            Threads::Threads
            $<$<CXX_COMPILER_ID:GNU>:stdc++exp>
            $<$<PLATFORM_ID:Linux>:dl>
            $<$<PLATFORM_ID:Android>:dl>
            ${OPENGL_LIBRARIES}
            $<$<PLATFORM_ID:Linux>:${X11_LIBRARIES}>
    )
endif()
### Luncher

add_executable(Luncher)

target_sources( Luncher PRIVATE
    main.cpp 
)


if(NO_CONSOLE)
    no_console(Luncher)
endif()

target_link_libraries(Luncher PRIVATE Engine)

# common (Not Web)
if(NOT EMSCRIPTEN)
    if(NOT WIN32)
    target_link_libraries(Luncher PRIVATE $<$<CXX_COMPILER_ID:Clang>:stdc++exp>)
    endif()

    target_link_libraries(Luncher PRIVATE $<$<CXX_COMPILER_ID:GNU>:stdc++exp>)
endif()

add_custom_command(TARGET Luncher POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy -t $<TARGET_FILE_DIR:Luncher> $<TARGET_RUNTIME_DLLS:Luncher>
  COMMAND_EXPAND_LISTS
)

apply_all_options(TARGETS Luncher)
apply_coverage_options(TARGETS Luncher)
apply_harden_options(TARGETS Luncher)
apply_sanitizer_options(TARGETS Luncher)

print_target_compile_options(Luncher)
print_target_compile_options(Engine)
