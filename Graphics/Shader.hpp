//rights preserve to www.learnopengl.com
#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <map>

#define UNIFORM_BLOCK_INDEX_PROJECTION_MATRICS 0
namespace wlEngine
{
class Shader
{
public:
    static std::map<std::string, Shader *> collection;
    static void loadShader(const std::string &name,
                           const std::string &vertexPath,
                           const std::string &fragmentPath);

    static void loadShader(const std::string &name,
                           const std::string &vertexPath,
                           const std::string &tessCtrlPath,
                           const std::string &tessEvalPath,
                           const std::string &geometryPath,
                           const std::string &fragmentPath);

    static void loadShader(const std::string &name,
                           const std::string &vertexPath,
                           const std::string &tessEvalPath,
                           const std::string &geometryPath,
                           const std::string &fragmentPath);

    static void deleteShader(const std::string &name);
    unsigned int ID;
    ~Shader();

    void use() const;
	bool hasTess() const;
    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setVec2(const std::string &name, const glm::vec2 &value) const
    {
        glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void setVec2(const std::string &name, float x, float y) const
    {
        glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
    }
    // ------------------------------------------------------------------------
    void setVec3(const std::string &name, const glm::vec3 &value) const
    {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void setVec3(const std::string &name, float x, float y, float z) const
    {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
    }
    // ------------------------------------------------------------------------
    void setVec4(const std::string &name, const glm::vec4 &value) const
    {
        glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void setVec4(const std::string &name, float x, float y, float z, float w)
    {
        glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
    }
    // ------------------------------------------------------------------------
    void setMat2(const std::string &name, const glm::mat2 &mat) const
    {
        glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void setMat3(const std::string &name, const glm::mat3 &mat) const
    {
        glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void setMat4(const std::string &name, const glm::mat4 &mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
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
    static void setUniform(const GLuint &location, const float &x, const float &y)
    {
        glUniform2f(location, x, y);
    }
    static void setUniform(const GLuint &location, const glm::vec3 &vec)
    {
        glUniform3fv(location, 1, &vec[0]);
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
        glUniform1i(location, v);
    }
    static void setUniform(const GLuint &location, const bool &v)
    {
        glUniform1i(location, v);
    }

private:
    Shader(const std::string &vertexPath,
           const std::string &fragmentPath);
    Shader(const std::string &vertexPath,
           const std::string &tessCtrlPath,
           const std::string &tessEvalPath,
           const std::string &geometryPath,
           const std::string &fragmentPath);
    Shader();
    GLuint createShaderFromFile(const std::string &path, const GLenum &type);
    GLuint createProgram(const GLuint &vertexShader, const GLuint &tessCtrlPath, const GLuint &tessEvalPath, const GLuint &geometryShader, const GLuint &fragmentShader);

    bool hasTessellation;
};
} // namespace wlEngine
