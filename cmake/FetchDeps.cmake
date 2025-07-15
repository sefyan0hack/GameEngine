
include(CPM)
include(helpers)
# deps :

    # add glm 1.0.1
CPMAddPackage("gh:g-truc/glm#1.0.1")

# stb_image v2.30
CPMAddPackage(
    NAME stb_image_header
    URL https://raw.githubusercontent.com/nothings/stb/5c205738c191bcb0abc65c4febfa9bd25ff35234/stb_image.h
    DOWNLOAD_ONLY YES
    DOWNLOAD_NO_EXTRACT TRUE
    DOWNLOAD_NAME stb_image.h
)

set(STB_IMG_HEADER ${stb_image_header_SOURCE_DIR})
add_library(stb_image INTERFACE)
target_include_directories(stb_image SYSTEM INTERFACE ${STB_IMG_HEADER})

CPMAddPackage(
    NAME khr
    URL https://raw.githubusercontent.com/KhronosGroup/EGL-Registry/main/api/KHR/khrplatform.h
    DOWNLOAD_ONLY YES
    DOWNLOAD_NO_EXTRACT TRUE
    DOWNLOAD_NAME khrplatform.h
)

if(NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/KHR/khrplatform.h)
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/KHR)

    file(RENAME
        ${CMAKE_CURRENT_BINARY_DIR}/_deps/khr-src/khrplatform.h 
        ${CMAKE_CURRENT_BINARY_DIR}/KHR/khrplatform.h
    )
endif()

# fetch Opengl headers

add_single_file(glext https://registry.khronos.org/OpenGL/api/GL/glext.h)

if(EMSCRIPTEN)
    add_single_file(gl3 https://registry.khronos.org/OpenGL/api/GLES3/gl3.h)
    add_single_file(gl2ext https://registry.khronos.org/OpenGL/api/GLES2/gl2ext.h)
elseif(UNIX)
    add_single_file(glxext https://registry.khronos.org/OpenGL/api/GL/glxext.h)
elseif(WIN32)
    add_single_file(wglext https://registry.khronos.org/OpenGL/api/GL/wglext.h)
endif()