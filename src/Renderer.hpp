#pragma once
#include <string>
class Renderer
{
    
private:
    Renderer();
public:
    ~Renderer();

public:
    static Renderer& Instance() ;
};
