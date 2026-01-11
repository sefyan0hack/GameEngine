# Engine/cmakelists.txt

include(helpers)
include(GenerateExportHeader)

add_subdirectory(core)
add_subdirectory(graphics)
add_subdirectory(inputs)
add_subdirectory(ui)

get_target_property(CORE_SRCS core SOURCES)
get_target_property(INPUT_SRCS inputs SOURCES)
get_target_property(GRAPHICS_SRCS graphics SOURCES)
get_target_property(UI_SRCS ui SOURCES)

add_library(Engine)
set_target_properties(Engine PROPERTIES POSITION_INDEPENDENT_CODE TRUE)

generate_export_header(Engine)

target_include_directories(Engine PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

target_precompile_headers(Engine PUBLIC "${CMAKE_SOURCE_DIR}/Engine/Platform.hpp")

target_sources( Engine
    PRIVATE
        ${CORE_SRCS}
        ${INPUT_SRCS}
        ${GRAPHICS_SRCS}
        ${UI_SRCS}
)

target_link_libraries(Engine
    PUBLIC
        core
        graphics
        inputs
        ui
)

if(MSVC)
    target_compile_options(Engine PRIVATE /wd4251 /wd4275)
endif()

if(ANDROID)
    add_library(native_app_glue STATIC ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)
    target_include_directories(native_app_glue PUBLIC ${ANDROID_NDK}/sources/android/native_app_glue)
endif()

# common (Not Web)
if(NOT EMSCRIPTEN)
    if(LINUX)
        target_link_libraries(Engine PRIVATE $<$<CXX_COMPILER_ID:Clang>:stdc++exp>)
    endif()

    target_link_libraries(Engine
        PRIVATE
            ${OPENGL_LIBRARIES}
            $<$<CXX_COMPILER_ID:GNU>:stdc++exp>
            $<$<PLATFORM_ID:Unix>:dl>
            $<$<PLATFORM_ID:Linux>:${X11_LIBRARIES}>
            $<$<PLATFORM_ID:Android>:android>
            $<$<PLATFORM_ID:Android>:EGL>
            $<$<PLATFORM_ID:Android>:GLESv3>
            $<$<PLATFORM_ID:Android>:log>
            $<$<PLATFORM_ID:Android>:native_app_glue>
    )
endif()
### Launcher

if(ANDROID)
    add_library(Launcher SHARED main.cpp)
    target_link_options(Engine PRIVATE "-Wl,-u,ANativeActivity_onCreate")

    set(OUT_LIB_ "${CMAKE_BINARY_DIR}/apk/lib/${ANDROID_ABI}")
    file(MAKE_DIRECTORY ${OUT_LIB_})
    set_target_properties(Launcher PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${OUT_LIB_})

else()
    add_executable(Launcher main.cpp)
endif()

target_link_libraries(Launcher PRIVATE Engine)

apply_all_options(TARGETS Engine Launcher)
apply_coverage_options(TARGETS Engine Launcher)
apply_sanitizer_options(TARGETS Engine Launcher)

print_target_compile_options(Launcher)
print_target_compile_options(Engine)
