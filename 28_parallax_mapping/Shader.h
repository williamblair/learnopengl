#ifndef SHADER_H_INCLUDED
#define SHADER_H_INCLUDED

#include <fstream>
#include <iostream>
#include <cstdlib>
#include <string>

#include <glad/glad.h>

typedef struct Shader {
    std::string fileName;
    std::string source; // the shader source code as a string
    unsigned int id;
} Shader;

static std::string loadShaderString(const std::string& fileName)
{
    std::ifstream shaderFile(fileName);
    if (!shaderFile.is_open()) {
        std::cout << "Failed to load shader file: "
            << fileName << std::endl;
        exit(EXIT_FAILURE);
    }

    std::string fileString = "";
    do
    {
        std::string currentLine;
        std::getline(shaderFile, currentLine);

        fileString += currentLine;
        fileString += "\n";

    } while (!shaderFile.eof());

    return fileString;
}

static void checkShaderCompileError(unsigned int shaderId)
{
    char infoLog[512];
    int  success;

    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shaderId, 512, NULL, infoLog);
        std::cout << "Failed to compile shader: "
            << infoLog << std::endl;
        exit(EXIT_FAILURE);
    }
}

Shader createVertexShader(const std::string& fileName) 
{
    Shader      vertexShader;
    const char* shaderSources[1];

    //std::string fileName = "vertexShader.glsl";

    vertexShader.fileName = fileName;
    vertexShader.source = loadShaderString(fileName);

    vertexShader.id = glCreateShader(GL_VERTEX_SHADER);
    shaderSources[0] = vertexShader.source.c_str();
    glShaderSource(vertexShader.id,
                    1,
                    shaderSources,
                    NULL);
    glCompileShader(vertexShader.id);
    checkShaderCompileError(vertexShader.id);

    return vertexShader;
}

Shader createFragmentShader(const std::string& fileName)
{
    Shader      fragmentShader;
    const char* shaderSources[1];

    //std::string fileName = "fragmentShader.glsl";

    fragmentShader.fileName = fileName;
    fragmentShader.source = loadShaderString(fileName);

    fragmentShader.id = glCreateShader(GL_FRAGMENT_SHADER);
    shaderSources[0] = fragmentShader.source.c_str();
    glShaderSource(fragmentShader.id,
        1,
        shaderSources,
        NULL);
    glCompileShader(fragmentShader.id);
    checkShaderCompileError(fragmentShader.id);

    return fragmentShader;
}

Shader createGeometryShader(const std::string& fileName) 
{
    Shader      geometryShader;
    const char* shaderSources[1];

    //std::string fileName = "geometryShader.glsl";

    geometryShader.fileName = fileName;
    geometryShader.source = loadShaderString(fileName);

    geometryShader.id = glCreateShader(GL_GEOMETRY_SHADER);
    shaderSources[0] = geometryShader.source.c_str();
    glShaderSource(geometryShader.id,
                    1,
                    shaderSources,
                    NULL);
    glCompileShader(geometryShader.id);
    checkShaderCompileError(geometryShader.id);

    return geometryShader;
}

#endif // !SHADER_H_INCLUDED
