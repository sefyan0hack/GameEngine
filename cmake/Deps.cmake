
include_directories( SYSTEM "${CMAKE_SOURCE_DIR}/3party")

add_subdirectory(3party/emath)

# freetype 2.14.0
add_subdirectory(3party/freetype)