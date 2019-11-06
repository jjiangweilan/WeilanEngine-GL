#include "Shader.hpp"

namespace wlEngine
{
std::map<std::string, Shader *> Shader::collection;

Shader::Shader() : Shader("texture.vert", "texture.frag")
{
}

Shader::Shader(const std::string &vertexPath, const std::string &fragmentPath)
{
    
    auto vertex = createShaderFromFile(vertexPath, GL_VERTEX_SHADER);
    auto fragment = createShaderFromFile(fragmentPath, GL_FRAGMENT_SHADER);
    ID = createProgram(vertex, fragment);
}

Shader::Shader(const std::string &vertexPath,
               const std::string &geometryPath,
               const std::string &fragmentPath)
{
    auto vertex = createShaderFromFile(vertexPath, GL_VERTEX_SHADER);
    auto geometry = createShaderFromFile(vertexPath, GL_GEOMETRY_SHADER);
    auto fragment = createShaderFromFile(fragmentPath, GL_FRAGMENT_SHADER);
    ID = createProgram(vertex, geometry, fragment);
}
GLuint Shader::createProgram(const GLuint &vertexShader,
                             const GLuint &geometryShader,
                             const GLuint &fragmentShader)
{
    GLuint ID = glCreateProgram();
    glAttachShader(ID, vertexShader);
    glAttachShader(ID, geometryShader);
    glAttachShader(ID, fragmentShader);
    glLinkProgram(ID);
    int success;
    char infoLog[1024];
    glGetShaderiv(ID, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(ID, 1024, NULL, infoLog);
        std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: "
                  << "PROGRAME"
                  << "\n"
                  << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(geometryShader);
    glDeleteShader(fragmentShader);
    return ID;
}

GLuint Shader::createProgram(const GLuint &vertexShader, const GLuint &fragmentShader)
{
    GLuint ID = glCreateProgram();
    glAttachShader(ID, vertexShader);
    glAttachShader(ID, fragmentShader);
    glLinkProgram(ID);
    int success;
    char infoLog[1024];
    glGetShaderiv(ID, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(ID, 1024, NULL, infoLog);
        std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: "
                  << "PROGRAME"
                  << "\n"
                  << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return ID;
}
void Shader::loadShader(const std::string &name, const std::string &vertexPath, const std::string &fragmentPath)
{
    collection.insert({name, new Shader(vertexPath, fragmentPath)});
}

void Shader::deleteShader(const std::string &name)
{
    auto iter = collection.find(name);
    if (iter != collection.end())
    {
        delete iter->second;
        collection.erase(iter);
    }
    else
    {
        assert(0 && "deleteShader: no such shader");
    }
}

void Shader::use() const
{
    glUseProgram(ID);
}

void Shader::setBool(const std::string &name, bool value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string &name, int value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string &name, float value) const
{
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::loadShader(const std::string &name, const std::string &vertexPath, const std::string &geometryPath, const std::string &fragmentPath)
{
}
Shader::~Shader()
{
    glDeleteProgram(ID);
}

GLuint Shader::createShaderFromFile(const std::string &path, const GLenum &type)
{
    std::ifstream shaderFile;
    std::string shaderCode;
    try
    {
        shaderFile.open(path);
        std::stringstream shaderStream;
        shaderStream << shaderFile.rdbuf();
        shaderFile.close();
        shaderCode = shaderStream.str();
    }
    catch (const std::exception &e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << path << std::endl;
        return 0;
    }

    GLuint shader;
    const char *code = shaderCode.data();
    shader = glCreateShader(type);
    glShaderSource(shader, 1, &code, nullptr);
    glCompileShader(shader);

    //check if compiling successed
    int success;
    char infoLog[1024];
    glGetProgramiv(shader, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shader, 1024, NULL, infoLog);
        std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n"
                  << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        return 0;
    }

    return shader;
}
} // namespace wlEngine
