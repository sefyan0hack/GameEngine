
include_directories("${CMAKE_SOURCE_DIR}/3party")

# add glm 1.0.1
add_subdirectory(3party/glm)

# add stb_image.h
set(STB_IMG_HEADER ${CMAKE_SOURCE_DIR}/3party)

add_library(stb_image INTERFACE)
target_include_directories(stb_image SYSTEM INTERFACE ${STB_IMG_HEADER})

# add freetype 2.14.0
add_subdirectory(3party/freetype)
