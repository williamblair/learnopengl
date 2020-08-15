#ifndef SHADER_PROGRAM_H_INCLUDED
#define SHADER_PROGRAM_H_INCLUDED

#include <iostream>

#include "Shader.h"

typedef struct ShaderProgram {
    const Shader* vertexShader;
    const Shader* fragmentShader;
    const Shader* geometryShader;
    unsigned int id;
} ShaderProgram;

static void checkShaderProgramCompileError(unsigned int id)
{
    int success;
    char infoLog[512];
    glGetProgramiv(id, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(id, 512, NULL, infoLog);
        std::cout << "Failed to compile shader program: "
            << infoLog << std::endl;
        exit(EXIT_FAILURE);
    }
}

ShaderProgram createShaderProgram(const Shader& vertexShader,
                                  const Shader& fragmentShader)
{
    ShaderProgram program;

    program.id = glCreateProgram();
    program.vertexShader = &vertexShader;
    program.fragmentShader = &fragmentShader;
    
    glAttachShader(program.id, vertexShader.id);
    glAttachShader(program.id, fragmentShader.id);
    glLinkProgram(program.id);

    checkShaderProgramCompileError(program.id);

    return program;
}

ShaderProgram createShaderProgram(const Shader& vertexShader,
                                  const Shader& geometryShader,
                                  const Shader& fragmentShader)
{
    ShaderProgram program;

    program.id = glCreateProgram();
    program.vertexShader = &vertexShader;
    program.fragmentShader = &fragmentShader;
    program.geometryShader = &geometryShader;
    
    glAttachShader(program.id, vertexShader.id);
    glAttachShader(program.id, geometryShader.id);
    glAttachShader(program.id, fragmentShader.id);
    glLinkProgram(program.id);

    checkShaderProgramCompileError(program.id);

    return program;
}

#endif // !SHADER_PROGRAM_H_INCLUDED

