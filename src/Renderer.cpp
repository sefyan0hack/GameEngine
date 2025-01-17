#include <glad/glad.h>
#include <core/Global_H.hpp>
#include <core/Renderer.hpp>

Renderer::Renderer(){
}
Renderer::~Renderer(){
}
auto Renderer::Instance() -> Renderer&
{
    static Renderer InsRendere;
    return InsRendere;
}
