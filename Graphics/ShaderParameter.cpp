#include "ShaderParameter.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "ShaderParameterDefines.hpp"
namespace wlEngine
{
namespace Graphics
{
void ShaderParameter::UpdateParameters(Shader *shader)
{
    m_parameters.clear();
    auto program = shader->getId();
    const GLsizei buffSize = 256;
    GLint count;
    GLchar name[buffSize];
    GLsizei length;
    GLenum type;
    GLint size;
    glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &count);

    for (int i = 0; i < count; i++)
    {
        glGetActiveUniform(program, (GLuint)i, buffSize, &length, &size, &type, name);
        std::string nameStr = std::string(name);
        switch (type)
        {
            MAKE_CASE(GL_FLOAT, float, Float, glGetUniformfv);
            MAKE_CASE(GL_DOUBLE, double, Double, glGetUniformdv);
            MAKE_CASE(GL_INT, int, Int, glGetUniformiv);
            MAKE_CASE(GL_UNSIGNED_INT, unsigned int, UInt, glGetUniformuiv);
            MAKE_CASE(GL_BOOL, int, Bool, glGetUniformiv);
            MAKE_CASE(GL_SAMPLER_2D, int, Sampler2D, glGetUniformiv);
            MAKE_CASE_VM(GL_FLOAT_VEC2, glm::vec2, Vec2, glGetUniformfv);
            MAKE_CASE_VM(GL_FLOAT_VEC3, glm::vec3, Vec3, glGetUniformfv);
            MAKE_CASE_VM(GL_FLOAT_VEC4, glm::vec4, Vec4, glGetUniformfv);
            MAKE_CASE_VM(GL_FLOAT_MAT2, glm::mat2, Mat2, glGetUniformfv);
            MAKE_CASE_VM(GL_FLOAT_MAT3, glm::mat3, Mat3, glGetUniformfv);
            MAKE_CASE_VM(GL_FLOAT_MAT4, glm::mat4, Mat4, glGetUniformfv);
        default:
            std::cerr << "type not supported in shader parameter: " << type << " " << name << std::endl;
            break;
        }
    }
}

ShaderParameter::ShaderParameter(const ShaderParameter &other)
{
    m_parameters.clear();
    for (auto &pair : other.m_parameters)
    {
        m_parameters.emplace(pair.first, pair.second->Clone());
    }
}
ShaderParameter::ShaderParameter(ShaderParameter &&other)
{
    m_parameters.clear();
    m_parameters = std::move(other.m_parameters);
}

ShaderParameter &ShaderParameter::operator=(const ShaderParameter &other)
{
    m_parameters.clear();
    for (auto &pair : other.m_parameters)
    {
        m_parameters.emplace(pair.first, pair.second->Clone());
    }
    return *this;
}
ShaderParameter &ShaderParameter::operator=(ShaderParameter &&other)
{
    m_parameters.clear();
    m_parameters = std::move(other.m_parameters);
    return *this;
}

void ShaderParameter::Use()
{
    for (auto &pair : m_parameters)
    {
        pair.second->Use();
    }
}

} // namespace Graphics
} // namespace wlEngine

namespace wlEngine
{
namespace Graphics
{
ShaderParameter::ShaderParameterBase::ShaderParameterBase(const unsigned int &loc, const ParameterType &type) : m_loc(loc), m_type(type)
{
}
void ShaderParameter::ShaderParameterBase::UpdateLocation(const unsigned int &loc)
{
    m_loc = loc;
}

ParameterType ShaderParameter::ShaderParameterBase::GetType()
{
    return m_type;
}
} // namespace Graphics
} // namespace wlEngine
