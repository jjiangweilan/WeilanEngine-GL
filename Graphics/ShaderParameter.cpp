#include "ShaderParameter.hpp"
#include <glm/glm.hpp>
#define MAKE_CASE(glType, storeType)                                                      \
    case glType:                                                                          \
        m_parameters.emplace(std::string(name), new ShaderParameterType<storeType>(loc)); \
        for (int i = 1; i < size; i++)                                                    \
        {                                                                                 \
            std::string nameStr = std::string(name);                                      \
            nameStr = nameStr.substr(0, nameStr.size() - 3);                              \
            nameStr += "[" + std::to_string(i) + "]";                                     \
            m_parameters.emplace(nameStr, new ShaderParameterType<storeType>(loc));       \
        }                                                                                 \
        break;

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

    for (int loc = 0; loc < count; loc++)
    {
        glGetActiveUniform(program, (GLuint)loc, buffSize, &length, &size, &type, name);
        switch (type)
        {
            MAKE_CASE(GL_FLOAT, float);
            MAKE_CASE(GL_FLOAT_VEC2, glm::vec2);
            MAKE_CASE(GL_FLOAT_VEC3, glm::vec3);
            MAKE_CASE(GL_FLOAT_VEC4, glm::vec4);
            MAKE_CASE(GL_DOUBLE, double);
            MAKE_CASE(GL_INT, int);
            MAKE_CASE(GL_UNSIGNED_INT, unsigned int);
            MAKE_CASE(GL_BOOL, bool);
            MAKE_CASE(GL_FLOAT_MAT2, glm::mat2);
            MAKE_CASE(GL_FLOAT_MAT3, glm::mat3);
            MAKE_CASE(GL_FLOAT_MAT4, glm::mat4);
            MAKE_CASE(GL_SAMPLER_2D, int);
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
	for (auto& pair : m_parameters)
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
ShaderParameter::ShaderParameterBase::ShaderParameterBase(const unsigned int &loc) : m_loc(loc)
{
}
void ShaderParameter::ShaderParameterBase::UpdateLocation(const unsigned int &loc)
{
    m_loc = loc;
}

} // namespace Graphics
} // namespace wlEngine
