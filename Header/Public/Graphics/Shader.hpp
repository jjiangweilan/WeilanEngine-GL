#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <functional>

namespace WeilanEngine
{
class GameEditor;
class Entity;
namespace Graphics
{
class Shader
{
public:
    ~Shader();
    Shader(Shader &&shader);
    Shader(const Shader &shader);
    void Use() const;
    bool hasTess() const;
    const GLuint &getPatches() const;
    const GLuint &getId() const;
    void Reload();
    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setVec2(const std::string &name, const glm::vec2 &value) const
    {
        glUniform2fv(glGetUniformLocation(m_id, name.c_str()), 1, &value[0]);
    }
    void setVec2(const std::string &name, float x, float y) const
    {
        glUniform2f(glGetUniformLocation(m_id, name.c_str()), x, y);
    }
    // ------------------------------------------------------------------------
    void setVec3(const std::string &name, const glm::vec3 &value) const
    {
        glUniform3fv(glGetUniformLocation(m_id, name.c_str()), 1, &value[0]);
    }
    void setVec3(const std::string &name, float x, float y, float z) const
    {
        glUniform3f(glGetUniformLocation(m_id, name.c_str()), x, y, z);
    }
    // ------------------------------------------------------------------------
    void setVec4(const std::string &name, const glm::vec4 &value) const
    {
        glUniform4fv(glGetUniformLocation(m_id, name.c_str()), 1, &value[0]);
    }
    void setVec4(const std::string &name, float x, float y, float z, float w)
    {
        glUniform4f(glGetUniformLocation(m_id, name.c_str()), x, y, z, w);
    }
    // ------------------------------------------------------------------------
    void setMat2(const std::string &name, const glm::mat2 &mat) const
    {
        glUniformMatrix2fv(glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void setMat3(const std::string &name, const glm::mat3 &mat) const
    {
        glUniformMatrix3fv(glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void setMat4(const std::string &name, const glm::mat4 &mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    static void setUniform(const GLuint &location, const glm::mat4 &mat)
    {
        glUniformMatrix4fv(location, 1, GL_FALSE, &mat[0][0]);
    }
    static void setUniform(const GLuint &location, const glm::mat3 &mat)
    {
        glUniformMatrix3fv(location, 1, GL_FALSE, &mat[0][0]);
    }
    static void setUniform(const GLuint &location, const glm::mat2 &mat)
    {
        glUniformMatrix2fv(location, 1, GL_FALSE, &mat[0][0]);
    }
    static void setUniform(const GLuint &location, const glm::vec2 &vec)
    {
        glUniform2fv(location, 1, &vec[0]);
    }
    static void setUniform(const GLuint &location, const glm::vec3 &vec)
    {
        glUniform3fv(location, 1, &vec[0]);
    }
    static void setUniform(const GLuint &location, const glm::vec4 &vec)
    {
        glUniform4fv(location, 1, &vec[0]);
    }
    static void setUniform(const GLuint &location, const float &x, const float &y)
    {
        glUniform2f(location, x, y);
    }
    static void setUniform(const GLuint &location, const float &x, const float &y, const float &z)
    {
        glUniform3f(location, x, y, z);
    }
    static void setUniform(const GLuint &location, const float &v)
    {
        glUniform1f(location, v);
    }
    static void setUniform(const GLuint &location, const int &v)
    {
        glUniform1i(location, v);
    }
    static void setUniform(const GLuint &location, const unsigned int &v)
    {
        glUniform1ui(location, v);
    }
    static void setUniform(const GLuint &location, const bool &v)
    {
        glUniform1i(location, v);
    }
    static void setUniform(const GLuint &location, const double &v)
    {
        glUniform1d(location, v);
    }

private:
    Shader(const std::string &vertexPath,
           const std::string &tessCtrlPath,
           const std::string &tessEvalPath,
           const std::string &geometryPath,
           const std::string &fragmentPath,
           const GLuint &patches = 0);

    GLuint createShaderFromFile(const std::string &path, const GLenum &type);
    GLuint createProgram(const GLuint &vertexShader, const GLuint &tessCtrlPath, const GLuint &tessEvalPath, const GLuint &geometryShader, const GLuint &fragmentShader);

    bool m_hasTessellation;
    GLuint m_patches;
    unsigned int m_id;

    std::string m_vertPath;
    std::string m_tessCtrlPath;
    std::string m_tessEvalPath;
    std::string m_geometryPath;
    std::string m_fragPath;

    /* Static ----*/
public:
    static Shader *get(const std::string &id);
    static Shader *Add(const std::string &id,
                       const std::string &vertexPath,
                       const std::string &fragmentPath);
    static Shader *AddWithMaterial(const std::string &id,
                                   const std::string &vertexPath,
                                   const std::string &fragmentPath);
    static Shader *Add(const std::string &name,
                       const std::string &vertexPath,
                       const std::string &tessCtrlPath,
                       const std::string &tessEvalPath,
                       const std::string &geometryPath,
                       const std::string &fragmentPath,
                       const GLuint &patches = 0);
    static Shader *AddWithMaterial(const std::string &name,
                                   const std::string &vertexPath,
                                   const std::string &tessCtrlPath,
                                   const std::string &tessEvalPath,
                                   const std::string &geometryPath,
                                   const std::string &fragmentPath,
                                   const GLuint &patches = 0);

    static void remove(const std::string &id);

private:
    static std::unordered_map<std::string, Shader> collection;

    friend class GameEditor;
};
} // namespace Graphics
} // namespace WeilanEngine
