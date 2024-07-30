#pragma once
#include <vector>
#include "Global_H.hpp"
NO_WARNING_BEGIN
#include <glad/glad.h>
NO_WARNING_END
class VBO
{
    public:
        VBO();
        ~VBO();
    public:
        void Bind() const;
        void UnBind() const;
        void UpData(const std::vector<float> &vertices);
    private:
        GLuint Id;
};
