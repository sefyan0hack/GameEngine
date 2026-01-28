
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
    if(NOT DEFINED ENV{ANDROID_SDK_ROOT})
        message(FATAL_ERROR "ANDROID_SDK_ROOT is not set")
    endif()

    if(NOT DEFINED ENV{JAVA_HOME})
        message(FATAL_ERROR "JAVA_HOME is not set (keytool requires a JDK)")
    endif()

    set(JAVA_HOME "$ENV{JAVA_HOME}")
    set(ANDROID_SDK_ROOT "$ENV{ANDROID_SDK_ROOT}")

    #--------------------------------
    file(GLOB ANDROID_BUILD_TOOLS_DIRS
        LIST_DIRECTORIES true
        "${ANDROID_SDK_ROOT}/build-tools/*")

    if(NOT ANDROID_BUILD_TOOLS_DIRS)
        message(FATAL_ERROR "No build found")
    endif()

    list(SORT ANDROID_BUILD_TOOLS_DIRS COMPARE NATURAL)
    list(REVERSE ANDROID_BUILD_TOOLS_DIRS)

    find_program(KEYTOOL
    NAMES keytool keytool.exe
    PATHS "${JAVA_HOME}/bin"
    NO_DEFAULT_PATH
    REQUIRED)

    find_program(AAPT
    NAMES aapt aapt.exe
    PATHS ${ANDROID_BUILD_TOOLS_DIRS}
    NO_DEFAULT_PATH
    REQUIRED)

    find_program(ZIPALIGN
    NAMES zipalign zipalign.exe
    PATHS ${ANDROID_BUILD_TOOLS_DIRS}
    NO_DEFAULT_PATH
    REQUIRED)

    find_program(APKSIGNER
    NAMES apksigner apksigner.bat
    PATHS ${ANDROID_BUILD_TOOLS_DIRS}
    NO_DEFAULT_PATH
    REQUIRED)

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

        COMMAND ${CMAKE_COMMAND} -E echo "Running AAPT packaging. ndk-min: ${ANDROID_NATIVE_API_LEVEL} ndk-max: ${NDK_MAX_PLATFORM_LEVEL}"
        COMMAND ${AAPT} package -f
            -I "${ANDROID_SDK_ROOT}/platforms/android-${NDK_MAX_PLATFORM_LEVEL}/android.jar"
            -M "${CMAKE_SOURCE_DIR}/cmake/AndroidManifest.xml"
            --min-sdk-version ${ANDROID_NATIVE_API_LEVEL} --target-sdk-version ${NDK_MAX_PLATFORM_LEVEL}
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

        COMMENT "Packaging & Signing APK abi: ${ANDROID_ABI} api-level: ${ANDROID_NATIVE_API_LEVEL} target-platform: ${ANDROID_PLATFORM}"
    )
endif()