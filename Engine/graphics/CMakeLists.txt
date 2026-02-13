
add_library(graphics INTERFACE)

collect_plt_sources(graphics)

target_sources( graphics
    PRIVATE
        ${PLT_SOURCE}
        ${CMAKE_CURRENT_SOURCE_DIR}/Window.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/APP.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Shader.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ShaderProgram.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Mesh.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/OpenGL.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Texture.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Scene.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Material.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/SkyBox.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Image.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Transform.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Camera.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/GameObject.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/OpenGLRenderer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/gl.cpp
    PUBLIC
    FILE_SET headers
      TYPE HEADERS
      BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
      FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/gl.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Window.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/APP.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Renderer.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Shader.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ShaderProgram.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Mesh.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/OpenGL.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Texture.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Scene.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Material.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Image.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/SkyBox.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Transform.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Camera.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/GameObject.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/OpenGLRenderer.hpp
)

target_include_directories(graphics 
    INTERFACE
        $<$<PLATFORM_ID:Linux>:${X11_INCLUDE_DIR}>
)

target_link_libraries(graphics INTERFACE res)
target_link_libraries(graphics INTERFACE glm::glm)
