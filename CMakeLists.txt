cmake_minimum_required(VERSION 3.25)

project(Engine LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_SUPPRESS_REGENERATION true)

if(NOT WIN32)
 message(FATAL_ERROR  "This project only support Windows. Sorry :)")
endif()

message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} (${CMAKE_CXX_COMPILER_VERSION})")

include(${CMAKE_SOURCE_DIR}/cmake/helpers.cmake)

if(EXISTS ${CMAKE_SOURCE_DIR}/3party)
    set(ONLINE FALSE)
else()
    set(ONLINE TRUE)
endif()

if(ONLINE)

# Include get_cpm.cmake
include(${CMAKE_SOURCE_DIR}/cmake/get_cpm.cmake)
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

CPMAddPackage(
    NAME glcorearb
    URL https://registry.khronos.org/OpenGL/api/GL/glcorearb.h
    DOWNLOAD_ONLY YES
    DOWNLOAD_NO_EXTRACT TRUE
    DOWNLOAD_NAME glcorearb.h
)

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
include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}/_deps/glcorearb-src  # For glcorearb.h
    ${CMAKE_CURRENT_BINARY_DIR}                      # For KHR/khrplatform.h
)
else()
add_subdirectory(3party/glm)
endif()

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
    src/Log.cpp
    src/ResourceManager.cpp
	)

# Find OpenGL package
find_package(OpenGL REQUIRED)

add_executable(${PROJECT_NAME} ${SOURCE})

if(ONLINE)
    list(APPEND SOURCES "${stb_image_header_SOURCE_DIR}/stb_image.h")
    set_source_files_properties("${stb_image_header_SOURCE_DIR}/stb_image.h" PROPERTIES COMPILE_FLAGS
        "$<$<CXX_COMPILER_ID:MSVC>:/W0>$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-w>"
    )

    target_include_directories(${PROJECT_NAME} PRIVATE
        "${CMAKE_SOURCE_DIR}/include"
        ${stb_image_header_SOURCE_DIR}
    )
else()
    list(APPEND SOURCES "${CMAKE_SOURCE_DIR}/3party/stb_image.h")
    set_source_files_properties("${CMAKE_SOURCE_DIR}/3party/stb_image.h" PROPERTIES COMPILE_FLAGS 
        "$<$<CXX_COMPILER_ID:MSVC>:/W0>$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-w>"
    )
  
    target_include_directories(${PROJECT_NAME} PRIVATE
        "${CMAKE_SOURCE_DIR}/include"
        "${CMAKE_SOURCE_DIR}/3party"
    )
endif()

# flags
apply_compile_options(TARGETS ${PROJECT_NAME} )

print_target_compile_options(${PROJECT_NAME})

target_compile_definitions(${PROJECT_NAME}
    PRIVATE
        PROJECT_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
        $<$<CONFIG:Debug>:DEBUG>
)

if(MSVC)
    target_link_libraries(${PROJECT_NAME}
    PRIVATE
        glm::glm
        ${OPENGL_LIBRARIES}
        $<$<CONFIG:Debug>:dbghelp>
    )
else()
    target_link_libraries(${PROJECT_NAME}
    PRIVATE 
        glm::glm 
        ${OPENGL_LIBRARIES} 
        stdc++exp 
        $<$<CONFIG:Debug>:dbghelp>
    )
endif()
