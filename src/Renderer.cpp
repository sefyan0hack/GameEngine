#include <glad/glad.h>
#include "Global_H.hpp"
#include "Renderer.hpp"

Renderer::Renderer(){
}
Renderer::~Renderer(){
}
Renderer& Renderer::Instance(){
    static Renderer InsRendere;
    return InsRendere;
}
