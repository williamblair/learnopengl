#ifndef SHADER_PROGRAM_H_INCLUDED
#define SHADER_PROGRAM_H_INCLUDED

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"

//typedef struct ShaderProgram {
//    const Shader* vertexShader;
//    const Shader* fragmentShader;
//    unsigned int id;
//} ShaderProgram;

class ShaderProgram
{
public:
    ShaderProgram() {}
    ~ShaderProgram() 
    {
        //glDeleteShader(vertexShader.id);
        //glDeleteShader(fragmentShader.id);
    }

    void Create(const std::string& vertexFileName, const std::string& fragmentFileName)
    {
        id = glCreateProgram();
        //program.vertexShader = &vertexShader;
        //program.fragmentShader = &fragmentShader;

        vertexShader = createVertexShader(vertexFileName);
        fragmentShader = createFragmentShader(fragmentFileName);

        glAttachShader(id, vertexShader.id);
        glAttachShader(id, fragmentShader.id);
        glLinkProgram(id);

        checkShaderProgramCompileError(id);
    }

    void Use()
    {
        glUseProgram(id);
    }

    void SetVec3fv(const char* name, const glm::vec3& vals) const
    {
        glUniform3fv(glGetUniformLocation(id, name), 1, glm::value_ptr(vals));
    }

    void SetVec1f(const char* name, const float val)
    {
        glUniform1f(glGetUniformLocation(id, name), val);
    }

    void SetVec1i(const char* name, const int val)
    {
        glUniform1i(glGetUniformLocation(id, name), val);
    }

    void SetMat4fv(const char* name, const glm::mat4& vals)
    {
        glUniformMatrix4fv(glGetUniformLocation(id, name),
            1, // number of matrices
            GL_FALSE, // should the matrices be transposed?
            glm::value_ptr(vals)); // pointer to data
    }

private:
    Shader vertexShader;
    Shader fragmentShader;
    unsigned int id;

    void checkShaderProgramCompileError(unsigned int id)
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
};

#endif // !SHADER_PROGRAM_H_INCLUDED

