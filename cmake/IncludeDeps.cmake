
include_directories( SYSTEM "${CMAKE_SOURCE_DIR}/3party")

# add glm 1.0.1
add_subdirectory(3party/glm)

# add freetype 2.14.0
add_subdirectory(3party/freetype)
