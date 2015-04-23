#ifndef SHADER_H
#define SHADER_H


#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
using namespace std;

extern "C"{
#define GLEW_STATIC
#include <GL/glew.h> // Include glew to get all the required OpenGL headers
}
class Shader
{
public:
        // The program ID
        GLuint Program;
        // Constructor reads and builds the shader
        Shader(const GLchar* vertexSourcePath, const GLchar* fragmentSourcePath);
        // Use the program
        void Use();
};

#endif // SHADER_H

