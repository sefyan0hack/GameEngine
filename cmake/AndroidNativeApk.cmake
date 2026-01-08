if(ANDROID)
    function(android_native_apk TARGET_NAME)
    if(NOT TARGET ${TARGET_NAME})
        message(FATAL_ERROR "Target '${TARGET_NAME}' does not exist")
    endif()

    # ---------- SDK ----------
    if(NOT ANDROID_SDK_ROOT)
        if(DEFINED ENV{ANDROID_SDK_ROOT})
            set(ANDROID_SDK_ROOT $ENV{ANDROID_SDK_ROOT})
        elseif(DEFINED ENV{ANDROID_HOME})
            set(ANDROID_SDK_ROOT $ENV{ANDROID_HOME})
        else()
            message(FATAL_ERROR "ANDROID_SDK_ROOT not found")
        endif()
    endif()

    file(GLOB _bt "${ANDROID_SDK_ROOT}/build-tools/*")
    list(SORT _bt)
    list(REVERSE _bt)
    list(GET _bt 0 ANDROID_BUILD_TOOLS)

    set(AAPT2     ${ANDROID_BUILD_TOOLS}/aapt2)
    set(ZIPALIGN  ${ANDROID_BUILD_TOOLS}/zipalign)
    set(APKSIGNER ${ANDROID_BUILD_TOOLS}/apksigner)

    # ---------- Defaults ----------
    set(ANDROID_MIN_SDK 21)
    set(ANDROID_TARGET_SDK 33)
    set(PACKAGE_NAME com.engine.nativeapp)
    set(APP_NAME NativeApp)

    # ---------- Paths ----------
    set(APK_DIR ${CMAKE_BINARY_DIR}/apk)
    set(LIB_DIR ${APK_DIR}/lib/${ANDROID_ABI})
    set(MANIFEST ${CMAKE_BINARY_DIR}/AndroidManifest.xml)
    set(KEYSTORE ${CMAKE_BINARY_DIR}/debug.keystore)

    file(MAKE_DIRECTORY ${LIB_DIR})

    # ---------- Manifest ----------
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
             android:value=\"${TARGET_NAME}\" />
         <intent-filter>
             <action android:name=\"android.intent.action.MAIN\"/>
             <category android:name=\"android.intent.category.LAUNCHER\"/>
         </intent-filter>
     </activity>
 </application>
</manifest>"
    )

    add_custom_command(
        TARGET ${TARGET_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            $<TARGET_FILE:${TARGET_NAME}>
            ${LIB_DIR}/$<TARGET_FILE_NAME:${TARGET_NAME}>
    )

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
    )

    set(UNSIGNED_APK ${CMAKE_BINARY_DIR}/app-unsigned.apk)
    set(ALIGNED_APK  ${CMAKE_BINARY_DIR}/app-aligned.apk)

    add_custom_target(apk
        DEPENDS ${TARGET_NAME} ${KEYSTORE}

        COMMAND ${AAPT2} link
            --manifest ${MANIFEST}
            --min-sdk-version ${ANDROID_MIN_SDK}
            --target-sdk-version ${ANDROID_TARGET_SDK}
            --auto-add-overlay
            -o ${UNSIGNED_APK}

        COMMAND ${CMAKE_COMMAND} -E chdir ${APK_DIR}
            zip -qr ${UNSIGNED_APK} lib

        COMMAND ${ZIPALIGN} -f 4 ${UNSIGNED_APK} ${ALIGNED_APK}
        COMMAND ${APKSIGNER} sign
            --ks ${KEYSTORE}
            --ks-pass pass:android
            --key-pass pass:android
            ${ALIGNED_APK}
    )
endfunction()

endif()