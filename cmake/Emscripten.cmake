
if(EMSCRIPTEN)

    add_link_options(
        -sMIN_WEBGL_VERSION=2
        -sFULL_ES3=1

        -sDISABLE_EXCEPTION_THROWING=0
        -sDISABLE_EXCEPTION_CATCHING=0
    )

endif()
