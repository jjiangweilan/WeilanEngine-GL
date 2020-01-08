
#include "Graphics/GlobalShaderParameter.hpp"

namespace wlEngine
{
namespace Graphics
{
GlobalShaderParameter::GlobalShaderParameter()
{
}
GlobalShaderParameter* GlobalShaderParameter::instance = nullptr;
GlobalShaderParameter *GlobalShaderParameter::Get()
{
    if (instance)
    {
        return instance;
    }
    instance = new GlobalShaderParameter();
    return instance;
}

GLuint GlobalShaderParameter::GenBlock(const std::string& name, const GLuint &index, const GLsizeiptr &size, const void* data)
{
	GLuint UBO;
    glGenBuffers(1, &UBO);
    glBindBuffer(GL_UNIFORM_BUFFER, UBO);
    glBufferData(GL_UNIFORM_BUFFER, size, data, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferBase(GL_UNIFORM_BUFFER, index, UBO);
	m_UBOs[name] = UBO;
    AddConfiguredShaderParameterBlock(name, index);
    return UBO;
}

void GlobalShaderParameter::SubData(const GLuint& UBO, const GLintptr &offset, const GLsizeiptr &size, const void *data)
{
    glBindBuffer(GL_UNIFORM_BUFFER, UBO);
    glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
void GlobalShaderParameter::AddConfiguredShaderParameterBlock(const std::string &name, const GLuint &index)
{
    m_namedBlocks.emplace_back(name, index);
}
void GlobalShaderParameter::ConfigureShaderParameterBlock(const GLuint& program)
{
    for (auto& block : m_namedBlocks)
    {
        auto uniformBlockIndex = glGetUniformBlockIndex(program, block.name.data());
        if (uniformBlockIndex != GL_INVALID_INDEX)
            glUniformBlockBinding(program, uniformBlockIndex, block.index);
    }
}
GLuint GlobalShaderParameter::GetGlobalParameterIndex(const std::string& name)
{
	return m_UBOs[name];
}
} // namespace Graphics
} // namespace wlEngine