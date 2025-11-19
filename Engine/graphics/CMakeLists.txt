# Engine/graphics/cmakelists.txt

include(helpers)

add_library(graphics INTERFACE)

target_sources( graphics PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Window.cpp    
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
)

target_sources( graphics PUBLIC
    FILE_SET HEADERS
      TYPE HEADERS
      FILES
        gl.h
        Window.hpp
        Renderer.hpp
        Shader.hpp
        ShaderProgram.hpp
        Mesh.hpp
        OpenGL.hpp
        Texture.hpp
        Scene.hpp
        Material.hpp
        Image.hpp
        SkyBox.hpp
        Transform.hpp
        Camera.hpp
        GameObject.hpp
        OpenGLRenderer.hpp
)

target_include_directories(graphics 
    INTERFACE 
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/Engine
        $<$<PLATFORM_ID:Linux>:${X11_INCLUDE_DIR}>
)

target_link_libraries(graphics INTERFACE res)
target_link_libraries(graphics INTERFACE glm::glm)
target_link_libraries(graphics INTERFACE stb_image)
