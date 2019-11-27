#include "Shader.hpp"
#include "GlobalShaderParameter.hpp"

namespace wlEngine
{
namespace Graphics
{

std::unordered_map<std::string, Shader> Shader::collection;
Shader::Shader(const std::string &vertexPath,
               const std::string &tessCtrlPath,
               const std::string &tessEvalPath,
               const std::string &geometryPath,
               const std::string &fragmentPath,
               const GLuint &patches) : m_hasTessellation(false), m_patches(patches), paramUpdateFunc(nullptr)
{
    GLuint vertex = 0, tessCtrl = 0, tessEval = 0, geometry = 0, fragment = 0;
    vertex = createShaderFromFile(vertexPath, GL_VERTEX_SHADER);
    fragment = createShaderFromFile(fragmentPath, GL_FRAGMENT_SHADER);

    tessCtrl = tessCtrlPath.size() == 0 ? 0 : createShaderFromFile(tessCtrlPath, GL_TESS_CONTROL_SHADER);
    tessEval = tessEvalPath.size() == 0 ? 0 : createShaderFromFile(tessEvalPath, GL_TESS_EVALUATION_SHADER);
    geometry = geometryPath.size() == 0 ? 0 : createShaderFromFile(geometryPath, GL_GEOMETRY_SHADER);
    m_id = createProgram(vertex, tessCtrl, tessEval, geometry, fragment);

    GlobalShaderParameter::Get()->ConfigureShaderParameterBlock(m_id);
}

Shader::Shader(Shader &&shader)
{
    m_hasTessellation = shader.m_hasTessellation;
    m_patches = shader.m_patches;
    m_id = shader.m_id;
    shader.m_id = 0;
    shader.m_patches = 0;
    shader.m_hasTessellation = 0;
}
Shader::Shader(const Shader &shader)
{
    m_hasTessellation = shader.m_hasTessellation;
    m_patches = shader.m_patches;
    m_id = shader.m_id;
}

GLuint Shader::createProgram(const GLuint &vertexShader,
                             const GLuint &tessCtrlShader,
                             const GLuint &tessEvalShader,
                             const GLuint &geometryShader,
                             const GLuint &fragmentShader)
{
    m_id = glCreateProgram();
    glAttachShader(m_id, vertexShader);
    glAttachShader(m_id, fragmentShader);

    if (tessCtrlShader != 0)
    {
        glAttachShader(m_id, tessCtrlShader);
    }

    if (tessEvalShader != 0)
    {
        glAttachShader(m_id, tessEvalShader);
        m_hasTessellation = true;
    }
    if (geometryShader != 0)
        glAttachShader(m_id, geometryShader);

    glLinkProgram(m_id);

    //error check
    GLint success = GL_FALSE;
    char infoLog[1024];
    glGetShaderiv(m_id, GL_LINK_STATUS, &success);
    if (success == GL_TRUE)
    {
        glGetShaderInfoLog(m_id, 1024, NULL, infoLog);
        std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: "
                  << "PROGRAME"
                  << "\n"
                  << infoLog << "------------------------------------------------------- " << std::endl;
    }

    //clean up
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    if (geometryShader)
        glDeleteShader(geometryShader);
    if (tessCtrlShader)
        glDeleteShader(tessCtrlShader);
    if (tessEvalShader)
        glDeleteShader(tessEvalShader);

    return m_id;
}

GLuint Shader::createShaderFromFile(const std::string &path, const GLenum &type)
{
    std::ifstream shaderFile;
    std::string shaderCode;
    shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
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
    GLint success = GL_FALSE;
    char infoLog[1024];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success != GL_TRUE)
    {
        glGetShaderInfoLog(shader, 1024, NULL, infoLog);
        std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n"
                  << infoLog << "file: " << path << std::endl
                  << "------------------------------------------------------- " << std::endl;
        return 0;
    }

    return shader;
}

bool Shader::hasTess() const
{
    return m_hasTessellation;
}

void Shader::Use() const
{
    glUseProgram(m_id);
}

void Shader::setBool(const std::string &name, bool value) const
{
    glUniform1i(glGetUniformLocation(m_id, name.c_str()), (int)value);
}

void Shader::setInt(const std::string &name, int value) const
{
    glUniform1i(glGetUniformLocation(m_id, name.c_str()), value);
}

void Shader::setFloat(const std::string &name, float value) const
{
    glUniform1f(glGetUniformLocation(m_id, name.c_str()), value);
}

Shader::~Shader()
{
    glDeleteProgram(m_id);
}

Shader *Shader::add(const std::string &id,
                    const std::string &vertexPath,
                    const std::string &fragmentPath,
                    const std::function<void(Entity*)>& paramUpdateFunc)
{
    auto has = collection.find(id);
    if (has != collection.end())
        return &has->second;

    auto pair = collection.emplace(std::make_pair(id, Shader(vertexPath, "", "", "", fragmentPath, 0)));
	pair.first->second.paramUpdateFunc = paramUpdateFunc; // std::function can't be moved
    return &pair.first->second;
}
Shader *Shader::get(const std::string &id)
{
    auto iter = collection.find(id);
    if (iter == collection.end())
        return nullptr;
    return &iter->second;
}

Shader *Shader::add(const std::string &id,
                    const std::string &vertexPath,
                    const std::string &tessCtrlPath,
                    const std::string &tessEvalPath,
                    const std::string &geometryPath,
                    const std::string &fragmentPath,
                    const GLuint& patches,
                    const std::function<void(Entity*)>& paramUpdateFunc)
{
    auto has = collection.find(id);
    if (has != collection.end())
        return &has->second;

    auto pair = collection.emplace(std::make_pair(id, Shader(vertexPath,
                                                             tessCtrlPath,
                                                             tessEvalPath,
                                                             geometryPath,
                                                             fragmentPath,
                                                             patches)));
	pair.first->second.paramUpdateFunc = paramUpdateFunc; // std::function can't be moved
    return &pair.first->second;
}

void Shader::remove(const std::string &id)
{
    collection.erase(id);
}

const GLuint &Shader::getId() const
{
    return m_id;
}

const GLuint &Shader::getPatches() const
{
    return m_patches;
}
} // namespace Graphics
} // namespace wlEngine
