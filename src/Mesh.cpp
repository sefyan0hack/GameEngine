#include "Mesh.hpp"

Mesh::Mesh(std::vector<float> vertices, std::vector<GLuint> indices, std::string ShaderName)
{
    m_specification.Bind(); // 1 - bind vao
    m_buffer.Bind(); // 2 - bind vbo
    m_buffer.UpData(vertices); // 3 - up data to vbo
    // rmember the stride is grouing for VAO but the same for all attribs
    m_specification.SetLout(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); // 4 - set attribs
    m_specification.SetLout(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3*sizeof(float))); // 4 - set attribs
    // vbo.UnBind(); // 5 - unbind if you want  vbo  (not vao)
    m_indices.Bind();
    m_indices.UpData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
    m_program.Load(ShaderName);
    m_PosLoc = m_program.GetUniformLocation("u_mat");
}

Mesh::~Mesh()
{
    m_specification.UnBind();
    m_indices.UnBind();
    m_buffer.UnBind();
    m_PosLoc = 0;
}

void Mesh::Bind()
{
    m_specification.Bind();
    m_indices.Bind();
    m_program.UseProgram();
}

GLuint Mesh::Getmat4Loc() const
{
    return m_PosLoc;
}
