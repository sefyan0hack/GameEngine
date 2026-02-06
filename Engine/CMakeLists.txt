
include(GenerateExportHeader)
include(Pack)

add_subdirectory(core)
add_subdirectory(graphics)
add_subdirectory(inputs)
add_subdirectory(ui)

add_library(Engine)

target_precompile_headers(Engine PUBLIC "${CMAKE_SOURCE_DIR}/Engine/Platform.hpp")

generate_export_header(Engine)
target_include_directories(Engine PUBLIC ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

target_sources( Engine
    PRIVATE
        $<TARGET_PROPERTY:core,SOURCES>
        $<TARGET_PROPERTY:inputs,SOURCES>
        $<TARGET_PROPERTY:graphics,SOURCES>
        $<TARGET_PROPERTY:ui,SOURCES>
    PUBLIC
    FILE_SET generated_headers
      TYPE HEADERS
      BASE_DIRS $<TARGET_PROPERTY:Engine,BINARY_DIR>
      FILES 
        ${CMAKE_CURRENT_BINARY_DIR}/engine_export.h
)

target_link_libraries(Engine
    PUBLIC
        core
        graphics
        inputs
        ui

        project_warnings_flags
        project_coverage_flags
        project_hardening_flags
)

if(ANDROID)
    add_library(native_app_glue STATIC ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)
    target_include_directories(native_app_glue PUBLIC ${ANDROID_NDK}/sources/android/native_app_glue)
endif()

target_link_libraries(Engine
    PRIVATE
        ${OPENGL_LIBRARIES}
        $<$<CXX_COMPILER_ID:GNU>:stdc++exp>
        $<$<AND:$<CXX_COMPILER_ID:Clang>,$<PLATFORM_ID:Linux>>:stdc++exp>

        $<$<PLATFORM_ID:Unix>:dl>
        $<$<PLATFORM_ID:Linux>:${X11_LIBRARIES}>

        $<$<PLATFORM_ID:Android>:
            android
            EGL
            GLESv3
            log
            native_app_glue
        >
)
target_compile_definitions( Engine PUBLIC

    $<$<PLATFORM_ID:Windows>:WINDOWS_PLT>
    $<$<PLATFORM_ID:Unix>:UNIX_PLT>
    $<$<PLATFORM_ID:Linux>:LINUX_PLT>
    $<$<PLATFORM_ID:Emscripten>:WEB_PLT>
    $<$<PLATFORM_ID:Android>:ANDROID_PLT>

    $<$<CONFIG:Debug>:DEBUG>
    $<$<CONFIG:Release>:NDEBUG>

    #this should be moved to Private
    $<$<OR:$<PLATFORM_ID:Windows>,$<PLATFORM_ID:Linux>>:CORE_GL>
    $<$<OR:$<PLATFORM_ID:Android>,$<PLATFORM_ID:Emscripten>>:ES_GL>
)

target_compile_definitions( Engine PRIVATE
    SYSTEM_HOST_NAME="${CMAKE_HOST_SYSTEM_NAME}"
    SYSTEM_HOST_VERSION="${CMAKE_HOST_SYSTEM_VERSION}"
    SYSTEM_HOST_ARCH="${CMAKE_HOST_SYSTEM_PROCESSOR}"

    COMPILER_NAME="${CMAKE_CXX_COMPILER_ID}"
    COMPILER_VERSION="${CMAKE_CXX_COMPILER_VERSION}"

    CXX_STANDARD="${CMAKE_CXX_STANDARD}"

    OPENGL_MODULE_NAME="${CORE_OPENGL_LIB}"

    DYN_LIB_PREFIX="${CMAKE_SHARED_LIBRARY_PREFIX}"
    DYN_LIB_SUFFIX="${CMAKE_SHARED_LIBRARY_SUFFIX}"

    $<$<BOOL:${GAME_HOT_RELOADABLE}>:GAME_HOT_RELOADABLE>
    $<$<BOOL:${CONSOLE_ATTACHED}>:CONSOLE_ATTACHED>
    $<$<BOOL:${ROBUST_GL_CHECK}>:ROBUST_GL_CHECK>

)

### Launcher
if(ANDROID)
    add_library(Launcher SHARED main.cpp)
    target_link_options(Engine PRIVATE "-Wl,-u,ANativeActivity_onCreate")
else()
    add_executable(Launcher main.cpp)
endif()

target_link_libraries(Launcher PRIVATE Engine)


target_pack(Launcher)