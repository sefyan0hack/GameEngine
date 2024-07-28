#pragma once
#include <vector>
class VBO
{
    public:
        VBO();
        VBO(const std::vector<float> &vertices);
        ~VBO();
    public:
        void Bind() const;
        void UnBind() const;
        void UpData(const std::vector<float> &vertices);
    private:
        GLuint Id;
};
