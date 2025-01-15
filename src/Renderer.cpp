#include <glad/glad.h>
#include "Global_H.hpp"
#include "Renderer.hpp"

Renderer::Renderer(){
}
Renderer::~Renderer(){
}
auto Renderer::Instance() -> Renderer&
{
    static Renderer InsRendere;
    return InsRendere;
}
