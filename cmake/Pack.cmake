function(target_pack target)
    if(EMSCRIPTEN)
        set(SITE_DIR "${CMAKE_BINARY_DIR}/site")

        add_custom_command(
            TARGET ${target}
            POST_BUILD

            COMMAND ${CMAKE_COMMAND} -E make_directory "${SITE_DIR}"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/cmake/index.html" "${SITE_DIR}"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:${target}>" "${SITE_DIR}"
            COMMENT "Assembling web site artifacts"
        )
    elseif(ANDROID)

        set(ANDROID_SDK_ROOT $ENV{ANDROID_SDK_ROOT})
        set(APK_LIB_DIR "${CMAKE_BINARY_DIR}/apk/lib/${ANDROID_ABI}")

        file(GLOB BUILD_TOOLS_VERSIONS RELATIVE "${ANDROID_SDK_ROOT}/build-tools" "${ANDROID_SDK_ROOT}/build-tools/*")
        list(SORT BUILD_TOOLS_VERSIONS)
        list(REVERSE BUILD_TOOLS_VERSIONS)
        list(GET BUILD_TOOLS_VERSIONS 0 BUILD_TOOLS_LATEST)
        set(BUILD_TOOLS_DIR "${ANDROID_SDK_ROOT}/build-tools/${BUILD_TOOLS_LATEST}")

        # highest available Android platform
        set(ANDROID_PLATFORM_LEVEL "")

        foreach(API RANGE 50 21 -1)
            if(EXISTS "${ANDROID_SDK_ROOT}/platforms/android-${API}/android.jar")
                set(ANDROID_PLATFORM_LEVEL ${API})
                break()
            endif()
        endforeach()

        if(NOT ANDROID_PLATFORM_LEVEL)
            message(FATAL_ERROR "No Android platform found in SDK")
        endif()

        find_program(AAPT aapt PATHS "${BUILD_TOOLS_DIR}" REQUIRED)
        find_program(ZIPALIGN NAMES zipalign zipalign.bat PATHS "${BUILD_TOOLS_DIR}" REQUIRED)
        find_program(APKSIGNER NAMES apksigner apksigner.bat PATHS "${BUILD_TOOLS_DIR}" REQUIRED)

        add_custom_command(
            TARGET ${target}
            POST_BUILD
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            VERBATIM

            COMMAND ${CMAKE_COMMAND} -E echo "Making APK lib dir: ${APK_LIB_DIR}"
            COMMAND ${CMAKE_COMMAND} -E make_directory "${APK_LIB_DIR}"

            COMMAND ${CMAKE_COMMAND} -E echo "Copying ${target} executable"
            COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${target}> "${APK_LIB_DIR}"

            COMMAND ${CMAKE_COMMAND} -E echo "Running AAPT packaging. ndk-min: ${NDK_MIN_PLATFORM_LEVEL} ndk-max: ${NDK_MAX_PLATFORM_LEVEL}"
            COMMAND ${AAPT} package -f
                -I "${ANDROID_SDK_ROOT}/platforms/android-${ANDROID_PLATFORM_LEVEL}/android.jar"
                -M "${CMAKE_SOURCE_DIR}/cmake/AndroidManifest.xml"
                --min-sdk-version ${NDK_MIN_PLATFORM_LEVEL} --target-sdk-version ${ANDROID_PLATFORM_LEVEL}
                -F base.apk ${CMAKE_BINARY_DIR}/apk

            COMMAND ${CMAKE_COMMAND} -E echo "Aligning APK"
            COMMAND ${ZIPALIGN} -f 4 base.apk aligned.apk

            COMMAND ${CMAKE_COMMAND} -E echo "Signing APK"
            COMMAND ${APKSIGNER} sign
                --ks ${CMAKE_SOURCE_DIR}/cmake/debug.keystore
                --ks-pass pass:android
                --key-pass pass:android
                --ks-key-alias androiddebugkey
                --out signed.apk aligned.apk

            COMMENT "Packaging & Signing APK. ndk: ${CMAKE_ANDROID_NDK_VERSION} sdk: ${ANDROID_PLATFORM_LEVEL} abi: ${ANDROID_ABI}"
        )
        endif()
endfunction()