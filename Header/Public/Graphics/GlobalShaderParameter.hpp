#pragma once
#include <glad/glad.h>
#include <string>
#include <vector>
#include <unordered_map>
/**
 * @brief Global Shader Parameter should be configured before Shader::add
 * 
 * 
 */
namespace WeilanEngine
{
namespace Graphics
{
class GlobalShaderParameter 
{
public:
    ~GlobalShaderParameter();
    
    /**
     * @brief create a uniform buffer with name
     * 
     * @param name 
     * @param index 
     * @param size 
     * @param data 
     * @return GLuint the generated uniform buffer
     */
    GLuint GenBlock(const std::string& name, const GLuint& index, const GLsizeiptr& size, const void* data);
    void SubData(const GLuint& UBO, const GLintptr& offset, const GLsizeiptr& size, const void* data);
	GLuint GetGlobalParameterIndex(const std::string& name);
    void ConfigureShaderParameterBlock(const GLuint& program);
    static GlobalShaderParameter* Get();
private:
    GlobalShaderParameter();
    void AddConfiguredShaderParameterBlock(const std::string& name, const GLuint& index);
    struct NamedGlobalParameterBlock
    {
        std::string name;
        GLuint index;
        NamedGlobalParameterBlock(const std::string& name, const GLuint& index) : name(name), index(index){}
    };

    std::unordered_map<std::string,GLuint > m_UBOs;

    std::vector<NamedGlobalParameterBlock> m_namedBlocks;

    static GlobalShaderParameter* instance;
};
} // namespace Graphics
} // namespace WeilanEngine
