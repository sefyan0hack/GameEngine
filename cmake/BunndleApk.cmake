if(ANDROID)
    # ================= ANDROID CONFIG =================
    set(ANDROID_MIN_SDK 21)
    set(ANDROID_TARGET_SDK 33)

    set(PACKAGE_NAME com.engine.nativeapp)
    set(APP_NAME NativeApp)
    set(NATIVE_LIB_NAME Launcher)

    # ================= ANDROID SDK =================
    if(NOT ANDROID_SDK_ROOT)
        if(DEFINED ENV{ANDROID_SDK_ROOT})
            set(ANDROID_SDK_ROOT $ENV{ANDROID_SDK_ROOT})
        elseif(DEFINED ENV{ANDROID_HOME})
            set(ANDROID_SDK_ROOT $ENV{ANDROID_HOME})
        else()
            message(FATAL_ERROR "ANDROID_SDK_ROOT not found")
        endif()
    endif()

    # Detect latest build-tools version
    file(GLOB ANDROID_BUILD_TOOLS_DIRS
        "${ANDROID_SDK_ROOT}/build-tools/*"
    )

    list(SORT ANDROID_BUILD_TOOLS_DIRS DESCENDING)
    list(GET ANDROID_BUILD_TOOLS_DIRS 0 ANDROID_BUILD_TOOLS)

    message(STATUS "Using Android build-tools: ${ANDROID_BUILD_TOOLS}")

    set(AAPT2     ${ANDROID_BUILD_TOOLS}/aapt2)
    set(ZIPALIGN  ${ANDROID_BUILD_TOOLS}/zipalign)
    set(APKSIGNER ${ANDROID_BUILD_TOOLS}/apksigner)

    # ================= PATHS =================
    set(APK_DIR ${CMAKE_BINARY_DIR}/apk)
    set(LIB_DIR ${APK_DIR}/lib/${ANDROID_ABI})
    set(MANIFEST ${CMAKE_BINARY_DIR}/AndroidManifest.xml)
    set(KEYSTORE ${CMAKE_BINARY_DIR}/debug.keystore)

    file(MAKE_DIRECTORY ${LIB_DIR})

    # ================= MANIFEST =================
    file(WRITE ${MANIFEST}
    "<?xml version=\"1.0\" encoding=\"utf-8\"?>
    <manifest xmlns:android=\"http://schemas.android.com/apk/res/android\"
        package=\"${PACKAGE_NAME}\">

        <application
            android:label=\"${APP_NAME}\"
            android:hasCode=\"false\">

            <activity
                android:name=\"android.app.NativeActivity\"
                android:screenOrientation=\"sensorLandscape\"
                android:configChanges=\"orientation|keyboardHidden|screenSize\"
                android:exported=\"true\">

                <meta-data
                    android:name=\"android.app.lib_name\"
                    android:value=\"${NATIVE_LIB_NAME}\" />

                <intent-filter>
                    <action android:name=\"android.intent.action.MAIN\"/>
                    <category android:name=\"android.intent.category.LAUNCHER\"/>
                </intent-filter>

            </activity>
        </application>
    </manifest>"
    )

    # ================= COPY NATIVE LIB =================
    add_custom_command(
        TARGET ${NATIVE_LIB_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            $<TARGET_FILE:${NATIVE_LIB_NAME}>
            ${LIB_DIR}/$<TARGET_FILE_NAME:${NATIVE_LIB_NAME}>
    )

    # ================= DEBUG KEYSTORE =================
    add_custom_command(
        OUTPUT ${KEYSTORE}
        COMMAND keytool -genkeypair
            -keystore ${KEYSTORE}
            -storepass android
            -keypass android
            -alias androiddebugkey
            -keyalg RSA
            -keysize 2048
            -validity 10000
        COMMENT "Generating debug keystore"
    )

    # ================= APK BUILD =================
    set(UNSIGNED_APK ${CMAKE_BINARY_DIR}/app-unsigned.apk)
    set(ALIGNED_APK  ${CMAKE_BINARY_DIR}/app-aligned.apk)

    add_custom_target(apk
        DEPENDS ${NATIVE_LIB_NAME} ${KEYSTORE}

        COMMAND ${AAPT2} link
            --manifest ${MANIFEST}
            --min-sdk-version ${ANDROID_MIN_SDK}
            --target-sdk-version ${ANDROID_TARGET_SDK}
            --auto-add-overlay
            -o ${UNSIGNED_APK}

        COMMAND ${CMAKE_COMMAND} -E chdir ${APK_DIR}
            zip -qr ${UNSIGNED_APK} lib

        COMMAND ${ZIPALIGN} -f 4
            ${UNSIGNED_APK}
            ${ALIGNED_APK}

        COMMAND ${APKSIGNER} sign
            --ks ${KEYSTORE}
            --ks-pass pass:android
            --key-pass pass:android
            ${ALIGNED_APK}

        COMMENT "Building signed APK (pure CMake, no Gradle)"
    )
endif()