
include(CPM)
include(helpers)

# add glm 1.0.1
CPMAddPackage("gh:g-truc/glm#1.0.1")

# stb_image v2.30
fetch_and_include_file(stb_image https://raw.githubusercontent.com/nothings/stb/5c205738c191bcb0abc65c4febfa9bd25ff35234/stb_image.h stb)

# fetch Opengl headers
if(WIN32)
  fetch_and_include_file(khrplatform https://registry.khronos.org/EGL/api/KHR/khrplatform.h KHR)
  fetch_and_include_file(glext https://registry.khronos.org/OpenGL/api/GL/glext.h GL)
endif()

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
