#pragma once
class VBO
{
    public:
        VBO(GLfloat *vertices, GLuint size);
        ~VBO();
    public:
        void Bind() const;
        void UnBind() const;
        void UpData(GLfloat *vertices, GLuint size);
    private:
        GLuint Id;
};
