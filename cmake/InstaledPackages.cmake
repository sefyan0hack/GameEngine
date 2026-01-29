
# Find OpenGL package
if(ANDROID)
    set(CORE_OPENGL_LIB GLESv3)
else()
    find_package(OpenGL QUIET)
    set_package_properties(OpenGL PROPERTIES
        URL "https://www.opengl.org"
        DESCRIPTION "A cross-language, cross-platform application programming interface for rendering 2D and 3D vector graphics."
        TYPE REQUIRED
        PURPOSE "Crucial for the rendering engine to work."
    )

    # Extract the core OpenGL library
    list(GET OPENGL_LIBRARIES 0 CORE_OPENGL_LIB)
endif()

# Find X11 package
if(LINUX)
    find_package(X11 QUIET)
    set_package_properties(XCB PROPERTIES
        DESCRIPTION "X11 Linux windowing lib"
        URL "https://x.org"
        TYPE REQUIRED
        PURPOSE "Required to to open windows in linux"
    )
endif()
