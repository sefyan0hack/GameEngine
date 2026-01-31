
include(GenerateExportHeader)

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
      FILES ${CMAKE_CURRENT_BINARY_DIR}/engine_export.h
)

target_link_libraries(Engine
    PUBLIC
        core
        graphics
        inputs
        ui

        project_warnings_flags
        project_coverage_flags
        project_sanitizer_flags
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
    $<$<CXX_COMPILER_ID:GNU>:GNU_CPL>
    $<$<CXX_COMPILER_ID:Clang>:CLANG_CPL>
    $<$<CXX_COMPILER_ID:MSVC>:MSVC_CPL>

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

    LINKER_NAME="${CMAKE_CXX_COMPILER_LINKER_ID}"
    LINKER_VERSION="${CMAKE_CXX_COMPILER_LINKER_VERSION}"

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
if(EMSCRIPTEN)
    set(SITE_DIR "${CMAKE_BINARY_DIR}/site")

    add_custom_command(
        TARGET Launcher
        POST_BUILD

        # Create site directory
        COMMAND ${CMAKE_COMMAND} -E make_directory "${SITE_DIR}"

        # Copy static index.html
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_SOURCE_DIR}/cmake/index.html"
            "${SITE_DIR}/index.html"

        # Copy local COOP / COEP service worker
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_SOURCE_DIR}/cmake/coi-serviceworker.js" 
            "${SITE_DIR}/coi-serviceworker.js"

        # Prevent GitHub Pages from using Jekyll
        COMMAND ${CMAKE_COMMAND} -E touch "${SITE_DIR}/.nojekyll"

        # Copy ALL Emscripten-generated outputs
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "$<TARGET_FILE_DIR:Launcher>"
            "${SITE_DIR}"

        COMMENT "Assembling web site artifacts"
    )
endif()

if(ANDROID)
    if(NOT DEFINED ANDROID_SDK_ROOT AND DEFINED ENV{ANDROID_SDK_ROOT})
        set(ANDROID_SDK_ROOT $ENV{ANDROID_SDK_ROOT})
        message(STATUS "ANDROID_SDK_ROOT not found getting it from env : ${ANDROID_SDK_ROOT}")
    elseif(NOT DEFINED ANDROID_SDK_ROOT AND DEFINED ENV{ANDROID_HOME})
        set(ANDROID_SDK_ROOT $ENV{ANDROID_HOME})
        message(STATUS "ANDROID_SDK_ROOT not found getting it from env : ${ANDROID_SDK_ROOT}")
    endif()

    # --- Pick the first build-tools directory ---
    file(GLOB BUILD_TOOLS_VERSIONS RELATIVE "${ANDROID_SDK_ROOT}/build-tools" "${ANDROID_SDK_ROOT}/build-tools/*")
    list(SORT BUILD_TOOLS_VERSIONS)
    list(REVERSE BUILD_TOOLS_VERSIONS)
    list(GET BUILD_TOOLS_VERSIONS 0 BUILD_TOOLS_LATEST)
    set(BUILD_TOOLS_DIR "${ANDROID_SDK_ROOT}/build-tools/${BUILD_TOOLS_LATEST}")

    message(STATUS "Using build-tools dir: ${BUILD_TOOLS_DIR}")

    find_program(KEYTOOL keytool REQUIRED)
    find_program(AAPT aapt PATHS "${BUILD_TOOLS_DIR}" REQUIRED)
    find_program(ZIPALIGN zipalign PATHS "${BUILD_TOOLS_DIR}" REQUIRED)
    find_program(APKSIGNER apksigner PATHS "${BUILD_TOOLS_DIR}" REQUIRED)

    set(APK_LIB_DIR "${CMAKE_BINARY_DIR}/apk/lib/${ANDROID_ABI}")

    add_custom_command(
        TARGET Launcher
        POST_BUILD
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        VERBATIM

        COMMAND ${CMAKE_COMMAND} -E echo "Making APK lib dir: ${APK_LIB_DIR}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${APK_LIB_DIR}"

        COMMAND ${CMAKE_COMMAND} -E echo "Copying Launcher executable"
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:Launcher> "${APK_LIB_DIR}"

        COMMAND ${CMAKE_COMMAND} -E echo "Generating debug keystore"
        COMMAND ${KEYTOOL} -genkeypair -keystore "debug.keystore"
            -storepass android -keypass android -alias androiddebugkey
            -dname "CN=Android Debug,O=Android,C=US" -storetype JKS
            -keyalg RSA -keysize 2048 -validity 10000

        COMMAND ${CMAKE_COMMAND} -E echo "Running AAPT packaging. ndk-min: ${NDK_MIN_PLATFORM_LEVEL} ndk-max: ${NDK_MAX_PLATFORM_LEVEL}"
        COMMAND ${AAPT} package -f
            -I "${ANDROID_SDK_ROOT}/platforms/android-${NDK_MAX_PLATFORM_LEVEL}/android.jar"
            -M "${CMAKE_SOURCE_DIR}/cmake/AndroidManifest.xml"
            --min-sdk-version ${NDK_MIN_PLATFORM_LEVEL} --target-sdk-version ${NDK_MAX_PLATFORM_LEVEL}
            -F base.apk ${CMAKE_BINARY_DIR}/apk

        COMMAND ${CMAKE_COMMAND} -E echo "Aligning APK"
        COMMAND ${ZIPALIGN} -f 4 base.apk aligned.apk

        COMMAND ${CMAKE_COMMAND} -E echo "Signing APK"
        COMMAND ${APKSIGNER} sign
            --ks debug.keystore
            --ks-pass pass:android
            --key-pass pass:android
            --ks-key-alias androiddebugkey
            --out signed.apk aligned.apk

        COMMENT "Packaging & Signing APK. ndk: ${CMAKE_ANDROID_NDK_VERSION} abi: ${ANDROID_ABI} min-sdk: ${NDK_MIN_PLATFORM_LEVEL} target-sdk: ${NDK_MAX_PLATFORM_LEVEL}"
    )
endif()