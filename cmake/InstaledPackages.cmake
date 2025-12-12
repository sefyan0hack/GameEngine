
if(NOT EMSCRIPTEN)
    # Find OpenGL package
    find_package(OpenGL QUIET)
    set_package_properties(OpenGL PROPERTIES
        URL "https://www.opengl.org"
        DESCRIPTION "A cross-language, cross-platform application programming interface for rendering 2D and 3D vector graphics."
        TYPE REQUIRED
        PURPOSE "Crucial for the rendering engine to work."
    )

    # Find Threads package
    find_package(Threads QUIET)
    set_package_properties(OpenGL PROPERTIES
        DESCRIPTION "Threading support lib  ` posix or any `"
        TYPE REQUIRED
        PURPOSE "support parallel work"
    )
    # Find X11 package
    if(UNIX)
        find_package(X11 QUIET)
        set_package_properties(XCB PROPERTIES
            DESCRIPTION "X11 Linux windowing lib"
            URL "https://x.org"
            TYPE REQUIRED
            PURPOSE "Required to  to open windows in linux"
        )
    endif()
    # Extract the core OpenGL library (first entry in the list)
    list(GET OPENGL_LIBRARIES 0 CORE_OPENGL_LIB)
endif()
