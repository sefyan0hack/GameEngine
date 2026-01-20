
if(EMSCRIPTEN)
    set(OPENGL_LIB_NAME "WebGL")
    set(STATIC_LINK OFF)

    add_compile_options(
        -Oz
        -Wno-character-conversion -Wno-dollar-in-identifier-extension
        -msimd128
        -fPIC
        -pthread
        -matomics
        -flto
        -fexperimental-library
    )

    add_link_options(
        -Oz
        -flto
        -pthread
        -matomics
        -sSINGLE_FILE=1
        -sASYNCIFY
        -sUSE_WEBGL2=1
        -sMIN_WEBGL_VERSION=1
        -sMAX_WEBGL_VERSION=2
        -sALLOW_MEMORY_GROWTH=1 # -pthread + ALLOW_MEMORY_GROWTH may run non-wasm code slowly
        -sFULL_ES3=1
        -sDISABLE_EXCEPTION_THROWING=0
        -sDISABLE_EXCEPTION_CATCHING=0
        -sFILESYSTEM=1 # needed for DynLib 
        -sPTHREAD_POOL_SIZE=4
        -sEXIT_RUNTIME=1
        -sUSE_PTHREADS=1
        -sERROR_ON_UNDEFINED_SYMBOLS=0
    )

    configure_file(
        ${CMAKE_SOURCE_DIR}/cmake/index_template.cmake.in
        ${CMAKE_BINARY_DIR}/index_html.cmake
        @ONLY
    )

    add_custom_command(
        OUTPUT ${CMAKE_BINARY_DIR}/index.html
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/index_html.cmake ${CMAKE_BINARY_DIR}/index.html
        DEPENDS ${CMAKE_BINARY_DIR}/index_html.cmake Launcher
        COMMENT "Generating index.html"
    )
    
    add_custom_target(generate_index_html ALL
        DEPENDS ${CMAKE_BINARY_DIR}/index.html
    )

    # COOP and COEP headers trick
    file(DOWNLOAD
        "https://raw.githubusercontent.com/gzuidhof/coi-serviceworker/master/coi-serviceworker.js"
        "${CMAKE_BINARY_DIR}/coi-serviceworker.js"
        SHOW_PROGRESS
    )

endif()
