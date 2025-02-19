#include <core/gl.h>
#include <core/Log.hpp>
#include <core/Renderer.hpp>
#include <core/Scene.hpp>
#include <core/GameObject.hpp>
#include <core/Camera.hpp>
#include <ranges>
#include <iostream>

// const char* getEnumName(GLenum value) {
//     switch(value) {
//         // Capabilities
//         case GL_BLEND: return "GL_BLEND";
//         case GL_DEPTH_TEST: return "GL_DEPTH_TEST";
//         case GL_CULL_FACE: return "GL_CULL_FACE";
//         case GL_STENCIL_TEST: return "GL_STENCIL_TEST";
//         case GL_SCISSOR_TEST: return "GL_SCISSOR_TEST";
//         case GL_RASTERIZER_DISCARD: return "GL_RASTERIZER_DISCARD";
//         // Add more enums as needed
//         default: return "Unknown";
//     }
// }

// void dumpOpenGLState() {
//     // Basic information
//     std::cout << "Vendor: " << glGetString(GL_VENDOR) << "\n";
//     std::cout << "Renderer: " << glGetString(GL_RENDERER) << "\n";
//     std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << "\n";
//     std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n\n";

//     // Enabled capabilities
//     const std::vector<GLenum> capabilities = {
//         GL_BLEND, GL_DEPTH_TEST, GL_CULL_FACE,
//         GL_STENCIL_TEST, GL_SCISSOR_TEST, GL_RASTERIZER_DISCARD
//     };
    
//     std::cout << "Capabilities:\n";
//     for(auto cap : capabilities) {
//         std::cout << "  " << getEnumName(cap) << ": " 
//                   << (glIsEnabled(cap) ? "Enabled" : "Disabled") << "\n";
//     }
//     std::cout << "\n";

//     // Current program and VAO
//     GLint currentProgram;
//     glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
//     std::cout << "Current Program: " << currentProgram << "\n";

//     GLint currentVAO;
//     glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &currentVAO);
//     std::cout << "Current VAO: " << currentVAO << "\n\n";

//     // Buffer bindings
//     std::cout << "Buffer Bindings:\n";
//     const std::vector<std::pair<GLenum, const char*>> bufferBindings = {
//         {GL_ARRAY_BUFFER_BINDING, "Array Buffer"},
//         {GL_ELEMENT_ARRAY_BUFFER_BINDING, "Element Array Buffer"},
//         {GL_UNIFORM_BUFFER_BINDING, "Uniform Buffer"}
//     };

//     for(const auto& [binding, name] : bufferBindings) {
//         GLint value;
//         glGetIntegerv(binding, &value);
//         std::cout << "  " << name << ": " << value << "\n";
//     }
//     std::cout << "\n";

//     // Texture bindings
//     GLint maxTextureUnits;
//     glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxTextureUnits);
//     GLint activeTexture;
//     glGetIntegerv(GL_ACTIVE_TEXTURE, &activeTexture);

//     std::cout << "Texture Bindings (non-zero):\n";
//     for(GLint unit = 0; unit < maxTextureUnits; ++unit) {
//         glActiveTexture(GL_TEXTURE0 + unit);
        
//         GLint texture2D;
//         glGetIntegerv(GL_TEXTURE_BINDING_2D, &texture2D);
//         if(texture2D != 0) {
//             std::cout << "  Unit " << unit << " (2D): " << texture2D << "\n";
//         }

//         GLint textureCube;
//         glGetIntegerv(GL_TEXTURE_BINDING_CUBE_MAP, &textureCube);
//         if(textureCube != 0) {
//             std::cout << "  Unit " << unit << " (Cube): " << textureCube << "\n";
//         }
//     }
//     glActiveTexture(activeTexture); // Restore original
//     std::cout << "\n";

//     // Framebuffer state
//     GLint drawFBO, readFBO;
//     glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &drawFBO);
//     glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &readFBO);
//     std::cout << "Framebuffers:\n"
//               << "  Draw: " << drawFBO << "\n"
//               << "  Read: " << readFBO << "\n\n";

//     // Viewport and scissor
//     GLint viewport[4];
//     glGetIntegerv(GL_VIEWPORT, viewport);
//     std::cout << "Viewport: [" << viewport[0] << ", " << viewport[1] << ", "
//               << viewport[2] << ", " << viewport[3] << "]\n";

//     GLint scissor[4];
//     glGetIntegerv(GL_SCISSOR_BOX, scissor);
//     std::cout << "Scissor: [" << scissor[0] << ", " << scissor[1] << ", "
//               << scissor[2] << ", " << scissor[3] << "]\n\n";

//     // Blend state
//     GLfloat blendColor[4];
//     glGetFloatv(GL_BLEND_COLOR, blendColor);
//     std::cout << "Blend Color: (" << blendColor[0] << ", " << blendColor[1]
//               << ", " << blendColor[2] << ", " << blendColor[3] << ")\n";

//     GLint blendSrc, blendDst;
//     glGetIntegerv(GL_BLEND_SRC_RGB, &blendSrc);
//     glGetIntegerv(GL_BLEND_DST_RGB, &blendDst);
//     std::cout << "Blend Func: " << blendSrc << " / " << blendDst << "\n\n";

//     // Depth state
//     GLfloat clearDepth;
//     glGetFloatv(GL_DEPTH_CLEAR_VALUE, &clearDepth);
//     std::cout << "Depth Clear Value: " << clearDepth << "\n";

//     GLint depthFunc;
//     glGetIntegerv(GL_DEPTH_FUNC, &depthFunc);
//     std::cout << "Depth Func: " << depthFunc << "\n\n";

//     // Stencil state
//     GLint stencilRef;
//     glGetIntegerv(GL_STENCIL_REF, &stencilRef);
//     std::cout << "Stencil Ref: " << stencilRef << "\n";
// }

Renderer::Renderer() {
}
Renderer::~Renderer(){
}


auto Renderer::render(Scene &scene, Camera &camera) -> void
{
    //Drwaing    
    auto objs = scene.GetGameObjects();

    // render sky box
    auto skybox = objs.front();
    auto skymaterial = skybox.GetMaterial();
    auto skymesh = skybox.GetMesh();

    glDepthFunc(GL_LEQUAL);
    skymaterial->Use();
    skymaterial->SetUniform("View", glm::mat4(glm::mat3(camera.GetView())));
    skymaterial->SetUniform("Perspective", camera.GetPerspective());
    draw(*skymesh.get());
    glDepthFunc(GL_LESS);
    
    for(auto &obj: objs | std::views::drop(1)){
        auto material = obj.GetMaterial();
        material->Use();
        material->SetUniform("View", camera.GetView());
        material->SetUniform("Perspective", camera.GetPerspective());
        material->SetUniform("cameraPos", camera.GetPosition());
        auto sizeIns = static_cast<GLsizei>(obj.GetInstancePos().size());
        auto mesh = obj.GetMesh();
        draw(*mesh.get(), sizeIns);
    }
}

auto Renderer::draw(const Mesh& mesh, GLsizei count) -> void
{
    // dumpOpenGLState();
    mesh.EnableAttribs();
    Log::Expect(mesh.VAO != 0, "VAO is 0");
    glBindVertexArray(mesh.VAO);
    if(count > 1){
        #ifdef USE_EBO
        glDrawElementsInstanced(GL_TRIANGLES, mesh.vInSize, GL_UNSIGNED_INT, 0, count);
        #else
        glDrawArraysInstanced(GL_TRIANGLES, 0, mesh.vInSize, count);
        #endif
    }else{
        #ifdef USE_EBO
        glDrawElements(GL_TRIANGLES, mesh.vInSize, GL_UNSIGNED_INT, 0);
        #else
        glDrawArrays(GL_TRIANGLES, 0, mesh.vInSize);
        #endif
    }
    mesh.DisableAttribs();
}