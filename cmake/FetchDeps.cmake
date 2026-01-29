
include(CPM)
include(helpers)


# add glm 1.0.1
CPMAddPackage("gh:g-truc/glm#1.0.1")

# stb_image v2.30
fetch_and_include_file(stb_image https://raw.githubusercontent.com/nothings/stb/5c205738c191bcb0abc65c4febfa9bd25ff35234/stb_image.h)

add_library(stb_image INTERFACE)
target_include_directories(stb_image SYSTEM INTERFACE ${stb_image_SOURCE_DIR})


# fetch Opengl headers
# fetch_and_include_file(khrplatform https://raw.githubusercontent.com/KhronosGroup/EGL-Registry/main/api/KHR/khrplatform.h)

# if(NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/KHR/khrplatform.h)
#     file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/KHR)
#     include_directories( SYSTEM ${CMAKE_CURRENT_BINARY_DIR})

#     file(RENAME
#         ${khrplatform_SOURCE_DIR}/khrplatform.h
#         ${CMAKE_CURRENT_BINARY_DIR}/KHR/khrplatform.h
#     )
# endif()

# fetch_and_include_file(glext https://registry.khronos.org/OpenGL/api/GL/glext.h)

# if(WIN32)
#     fetch_and_include_file(wglext https://registry.khronos.org/OpenGL/api/GL/wglext.h)
# endif()

# if(LINUX)
#     fetch_and_include_file(glxext https://registry.khronos.org/OpenGL/api/GL/glxext.h)
# endif()

# freetype 2-14-0
CPMAddPackage(
  NAME freetype
  GITHUB_REPOSITORY freetype/freetype
  GIT_TAG VER-2-14-0
  OPTIONS
    "FT_DISABLE_HARFBUZZ ON"
    "FT_DISABLE_PNG ON"
    "FT_DISABLE_BZIP2 ON"
    "FT_DISABLE_ZLIB OFF"
)
