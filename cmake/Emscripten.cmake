
if(EMSCRIPTEN)
    set(OPENGL_LIB_NAME "WebGL")
    set(STATIC_LINK OFF)

    add_compile_options(
        -Wno-dollar-in-identifier-extension
        -fexperimental-library
        -pthread
    )

    add_link_options(
        -sUSE_PTHREADS=1
        -sPTHREAD_POOL_SIZE=4

        -sUSE_WEBGL2=1
        -sFULL_ES3=1
        -sMIN_WEBGL_VERSION=1
        -sMAX_WEBGL_VERSION=2

        -sINITIAL_MEMORY=268435456
    
        -sDISABLE_EXCEPTION_THROWING=0
        -sDISABLE_EXCEPTION_CATCHING=0

        -sFILESYSTEM=1 # needed for DynLib 

        -sGL_DEBUG=1
    )

endif()
