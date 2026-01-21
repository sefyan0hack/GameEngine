
if(EMSCRIPTEN)
    set(OPENGL_LIB_NAME "WebGL")
    set(STATIC_LINK OFF)

    add_compile_options(
        -Wno-character-conversion -Wno-dollar-in-identifier-extension
        -fexperimental-library
    )

    add_link_options(
        -sUSE_WEBGL2=1
        -sFULL_ES3=1
        -sMIN_WEBGL_VERSION=1
        -sMAX_WEBGL_VERSION=2
        -sALLOW_MEMORY_GROWTH=1
        -sDISABLE_EXCEPTION_THROWING=0
        -sDISABLE_EXCEPTION_CATCHING=0
        -sFILESYSTEM=1 # needed for DynLib 
        -sGL_DEBUG=1
        -sASSERTIONS=2
    )

endif()
