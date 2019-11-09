#include "Shader.hpp"

namespace wlEngine
{
std::map<std::string, Shader *> Shader::collection;

Shader::Shader() : Shader("texture.vert", "", "", "", "texture.frag")
{
}

Shader::Shader(const std::string &vertexPath,
               const std::string &tessCtrlPath,
               const std::string &tessEvalPath,
               const std::string &geometryPath,
               const std::string &fragmentPath) : hasTessellation(false)
{
    GLuint vertex = 0, tessCtrl = 0, tessEval = 0, geometry = 0, fragment = 0;
    vertex = createShaderFromFile(vertexPath, GL_VERTEX_SHADER);
    fragment = createShaderFromFile(fragmentPath, GL_FRAGMENT_SHADER);

    tessCtrl = tessCtrlPath.size() == 0 ? 0 : createShaderFromFile(tessCtrlPath, GL_TESS_CONTROL_SHADER);
    tessEval = tessEvalPath.size() == 0 ? 0 : createShaderFromFile(tessEvalPath, GL_TESS_EVALUATION_SHADER);
    geometry = geometryPath.size() == 0 ? 0 : createShaderFromFile(geometryPath, GL_GEOMETRY_SHADER);
    ID = createProgram(vertex, tessCtrl, tessEval, geometry, fragment);
	//uniform block
    auto matricesIndex = glGetUniformBlockIndex(ID, "_ProjMatrices");
    if(matricesIndex != GL_INVALID_INDEX) glUniformBlockBinding(ID, matricesIndex,UNIFORM_BLOCK_INDEX_PROJECTION_MATRICS);
}
GLuint Shader::createProgram(const GLuint &vertexShader,
                             const GLuint &tessCtrlShader,
                             const GLuint &tessEvalShader,
                             const GLuint &geometryShader,
                             const GLuint &fragmentShader)
{
    GLuint ID = glCreateProgram();
    glAttachShader(ID, vertexShader);
    glAttachShader(ID, fragmentShader);

	if (tessCtrlShader != 0)
	{
		glAttachShader(ID, tessCtrlShader);
	}
     
    if (tessEvalShader != 0)
    {
        glAttachShader(ID, tessEvalShader);
		hasTessellation = true;
    }
    if (geometryShader != 0)
        glAttachShader(ID, geometryShader);

    glLinkProgram(ID);

    //error check
    GLint success = GL_FALSE;
    char infoLog[1024];
    glGetShaderiv(ID, GL_LINK_STATUS, &success);
    if (success == GL_TRUE)
    {
        glGetShaderInfoLog(ID, 1024, NULL, infoLog);
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

    return ID;
}

void Shader::loadShader(const std::string &name, const std::string &vertexPath, const std::string &fragmentPath)
{

    auto shader = new Shader(vertexPath, "", "", "", fragmentPath);
    collection.insert({name, shader});
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

void Shader::loadShader(const std::string &name,
                        const std::string &vertexPath,
                        const std::string &tessCtrlPath,
                        const std::string &tessEvalPath,
                        const std::string &geometryPath,
                        const std::string &fragmentPath)
{
    auto shader = new Shader(vertexPath, tessCtrlPath, tessEvalPath, geometryPath, fragmentPath);
    collection[name] = shader;
}

void Shader::loadShader(const std::string &name,
                        const std::string &vertexPath,
                        const std::string &tessEvalPath,
                        const std::string &geometryPath,
                        const std::string &fragmentPath)
{
    auto shader = new Shader(vertexPath, "", tessEvalPath, geometryPath, fragmentPath);
    collection[name] = shader;
}

bool Shader::hasTess() const
{
	return hasTessellation;
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

Shader::~Shader()
{
    glDeleteProgram(ID);
}
} // namespace wlEngine
