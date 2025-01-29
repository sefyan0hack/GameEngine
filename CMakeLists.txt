cmake_minimum_required(VERSION 3.25)

project(Engine LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)


# Print compiler information
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} (${CMAKE_CXX_COMPILER_VERSION})")


set(CMAKE_MODULE_PATH "${CMAKE_BINARY_DIR}" "${CMAKE_MODULE_PATH}")
set(CMAKE_PREFIX_PATH "${CMAKE_BINARY_DIR}" "${CMAKE_PREFIX_PATH}")

set(SOURCE 
    src/main.cpp
    src/Keyboard.cpp
    src/Mouse.cpp
	src/Window.cpp
	src/Renderer.cpp
	src/Shader.cpp
	src/Transform.cpp
    src/Mesh.cpp
    src/Camera.cpp
    src/OpenGL.cpp
    src/APP.cpp
    src/Utils.cpp
    src/GameObject.cpp
    src/Texture.cpp
    src/Scene.cpp
    src/Material.cpp
	)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")

# Find OpenGL package
find_package(OpenGL REQUIRED)

# Add include directorys
include_directories("${CMAKE_SOURCE_DIR}/include")
include_directories(SYSTEM "${CMAKE_SOURCE_DIR}/3party")

# Add glad source file
add_library(glad STATIC 3party/glad/glad.c)
set_target_properties(glad PROPERTIES LINKER_LANGUAGE C)

# Add stb image source file
add_library(stb_image STATIC 3party/stb/stb_image.c)
set_target_properties(stb_image PROPERTIES LINKER_LANGUAGE C)


# remove some warning
add_executable(${PROJECT_NAME} ${SOURCE})
target_link_libraries(${PROJECT_NAME} glad stb_image  ${OPENGL_LIBRARIES})

if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE
        # Warnings
        /W4

        # Debug flags
        "$<$<CONFIG:Debug>:/Zi>"
        "$<$<CONFIG:Debug>:/Od>"

        # Release flags
        "$<$<CONFIG:Release>:/O2>"
        "$<$<CONFIG:Release>:/DNDEBUG>"
    )
else()
    target_compile_options(${PROJECT_NAME} PRIVATE
        # Warnings
        -Wall -Wextra -Wpedantic -Wconversion -Wno-cast-function-type

        # Debug flags
        "$<$<CONFIG:Debug>:-g>"
        "$<$<CONFIG:Debug>:-O0>"

        # Release flags
        "$<$<CONFIG:Release>:-O3>"
        "$<$<CONFIG:Release>:-DNDEBUG>"
        "$<$<CONFIG:Release>:-march=native>"
    )

endif()

function(print_target_compile_options target)
    foreach(config IN ITEMS ${CMAKE_BUILD_TYPE})
        get_target_property(flags ${target} COMPILE_OPTIONS)
        if(flags)
            string(REPLACE ";" " " flags_with_spaces "${flags}")
            message(STATUS "  + ${target} (${config}): ${flags_with_spaces}\n")
        else()
            message(STATUS "  + ${target} (${config}): No flags set\n")
        endif()
    endforeach()
endfunction()

print_target_compile_options(${PROJECT_NAME})
print_target_compile_options(glad)
print_target_compile_options(stb_image)

target_compile_definitions(${PROJECT_NAME}
    PRIVATE
        PROJECT_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
)

include(ProcessorCount)
ProcessorCount(NUMBER_OF_PROCESSORS)
if(NOT NUMBER_OF_PROCESSORS EQUAL 0)
	set(MAKEFLAGS "-j${NUMBER_OF_PROCESSORS}")
	set(ENV{MAKEFLAGS} "${MAKEFLAGS}")
	MESSAGE(STATUS "Set MAKEFLAGS for MinGW Makefiles to ${MAKEFLAGS}")
endif()


MARK_AS_ADVANCED(MULTITHREADED_BUILD)
SET(MULTITHREADED_BUILD ON CACHE BOOL "Parallel build with as many threads as possible!")
if(MULTITHREADED_BUILD)
    if(${CMAKE_GENERATOR} MATCHES "MinGW Makefiles")
        # Use MAKEFLAGS to specify the number of jobs for parallel build
        set(MAKEFLAGS "-j${NUMBER_OF_PROCESSORS}")
        set(ENV{MAKEFLAGS} "${MAKEFLAGS}")
        MESSAGE(STATUS "Set MAKEFLAGS for MinGW Makefiles to ${MAKEFLAGS}")
    elseif(MSVC)
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
        MESSAGE(STATUS "Added parallel build arguments to CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
    endif()
endif()