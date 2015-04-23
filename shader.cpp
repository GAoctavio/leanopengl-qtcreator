#include "shader.h"
Shader::Shader(const GLchar* vertexPath, const GLchar* fragmentPath)
{
    // 1. Retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    try
    {
        // Open files
        std::ifstream vShaderFile(vertexPath);
        std::ifstream fShaderFile(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        // Read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        // close file handlers
        vShaderFile.close();
        fShaderFile.close();
        // Convert stream into GLchar array
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch(std::exception e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }
    // 2. Compile shaders

    GLuint vertex, fragment;
    GLint success;
    GLchar infoLog[512];
    GLchar * vertexcodeCstr=(GLchar*)vertexCode.c_str();
    GLchar * fragmentcodeCstr=(GLchar*)fragmentCode.c_str();

    // Vertex Shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertexcodeCstr, NULL);
    glCompileShader(vertex);
    // Print compile errors if any
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    };
    // Similiar for Fragment Shader
    fragment= glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragmentcodeCstr, NULL);
    glCompileShader(fragment);
    // Check for compile time errors
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    this->Program = glCreateProgram();
    glAttachShader(this->Program, vertex);
    glAttachShader(this->Program, fragment);
    glLinkProgram(this->Program);
    glGetProgramiv(this->Program, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(this->Program, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    // Delete the shaders as they're linked into our program now and no longer necessery
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}
void Shader::Use(){
     glUseProgram(this->Program);
}
