
if(EMSCRIPTEN)
    set(OPENGL_LIB_NAME "WebGL")
    set(STATIC_LINK OFF)

    add_compile_options(
        -Wno-character-conversion -Wno-dollar-in-identifier-extension
        -fexperimental-library
    )

    add_link_options(
        -sUSE_WEBGL2=1
        -sMIN_WEBGL_VERSION=1
        -sMAX_WEBGL_VERSION=2
        -sALLOW_MEMORY_GROWTH=1 # -pthread + ALLOW_MEMORY_GROWTH may run non-wasm code slowly
        -sFULL_ES3=1
        -sFILESYSTEM=1 # needed for DynLib 
        -sEXIT_RUNTIME=1
        -sGL_DEBUG=1
    )

endif()
