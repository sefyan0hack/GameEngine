
include(GenerateExportHeader)

add_subdirectory(core)
add_subdirectory(graphics)
add_subdirectory(inputs)
add_subdirectory(ui)

add_library(Engine)

generate_export_header(Engine)

target_sources(Engine
    PRIVATE
        $<TARGET_PROPERTY:core,SOURCES>
        $<TARGET_PROPERTY:inputs,SOURCES>
        $<TARGET_PROPERTY:graphics,SOURCES>
        $<TARGET_PROPERTY:ui,SOURCES>
        $<$<PLATFORM_ID:Android>:${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c>
    PUBLIC
    FILE_SET generated_headers
      TYPE HEADERS
      BASE_DIRS ${CMAKE_CURRENT_BINARY_DIR}
      FILES 
        ${CMAKE_CURRENT_BINARY_DIR}/engine_export.h
    PUBLIC
    FILE_SET headers
      TYPE HEADERS
      BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
      FILES 
        ${CMAKE_CURRENT_SOURCE_DIR}/Engine.hpp
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

target_include_directories(Engine 
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE
        $<$<PLATFORM_ID:Android>:${ANDROID_NDK}/sources/android/native_app_glue>
)

target_link_libraries(Engine
    PRIVATE
        ${OPENGL_LIBRARIES}
        $<$<CXX_COMPILER_ID:GNU>:stdc++exp>
        $<$<AND:$<CXX_COMPILER_ID:Clang>,$<PLATFORM_ID:Linux>>:stdc++exp>

        $<$<BOOL:${Unix}>:dl>
        $<$<PLATFORM_ID:Linux>:${X11_LIBRARIES}>

        $<$<PLATFORM_ID:Android>:
            android
            EGL
            GLESv3
            log
        >
        $<$<PLATFORM_ID:Emscripten>:
            -sMIN_WEBGL_VERSION=2
            -sFULL_ES3=1

            -sALLOW_MEMORY_GROWTH=1

            -sDISABLE_EXCEPTION_THROWING=0
            -sDISABLE_EXCEPTION_CATCHING=0
        >
)

target_compile_definitions(Engine PUBLIC
    $<$<OR:$<PLATFORM_ID:Windows>,$<PLATFORM_ID:Linux>>:CORE_GL>
    $<$<OR:$<PLATFORM_ID:Android>,$<PLATFORM_ID:Emscripten>>:ES_GL>
)

target_compile_definitions(Engine PRIVATE
    SYSTEM_HOST_NAME="${CMAKE_HOST_SYSTEM_NAME}"
    SYSTEM_HOST_VERSION="${CMAKE_HOST_SYSTEM_VERSION}"
    SYSTEM_HOST_ARCH="${CMAKE_HOST_SYSTEM_PROCESSOR}"

    COMPILER_NAME="${CMAKE_CXX_COMPILER_ID}"
    COMPILER_VERSION="${CMAKE_CXX_COMPILER_VERSION}"

    CXX_STANDARD="${CMAKE_CXX_STANDARD}"

    OPENGL_MODULE_NAME="${OPENGL_gl_LIBRARY}"

    DYN_LIB_PREFIX="${CMAKE_SHARED_LIBRARY_PREFIX}"
    DYN_LIB_SUFFIX="${CMAKE_SHARED_LIBRARY_SUFFIX}"

    $<$<BOOL:${CONSOLE_ATTACHED}>:CONSOLE_ATTACHED>
    $<$<BOOL:${ROBUST_GL_CHECK}>:ROBUST_GL_CHECK>
    $<$<BOOL:${OBJ_LIFETIME_TRACE}>:OBJ_LIFETIME_TRACE>
)