#ifndef SHADER_PROGRAM_H_INCLUDED
#define SHADER_PROGRAM_H_INCLUDED

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"

class Mesh; // TODO - how to do includes for this properly? (without circular reference?)
class ShaderProgram
{
friend class Mesh; // allow mesh to access internal ID value
public:
    ShaderProgram() :
        hasGeometryShader(false)
    {
    }
    ~ShaderProgram() 
    {
        //glDeleteShader(vertexShader.id);
        //glDeleteShader(fragmentShader.id);
    }

    void Create(const std::string& vertexFileName,
                const std::string& fragmentFileName,
                const std::string& geometryFileName = "")
    {
        id = glCreateProgram();
        //program.vertexShader = &vertexShader;
        //program.fragmentShader = &fragmentShader;

        vertexShader = createVertexShader(vertexFileName);
        fragmentShader = createFragmentShader(fragmentFileName);
        glAttachShader(id, vertexShader.id);
        glAttachShader(id, fragmentShader.id);

        if (geometryFileName.size() > 0) {
            std::cout << "Creating geometry shader: " 
                      << geometryFileName << std::endl;
            geometryShader = createGeometryShader(geometryFileName);
            hasGeometryShader = true;        
            glAttachShader(id, geometryShader.id);
        }

        glLinkProgram(id);

        checkShaderProgramCompileError(id);
    }

    void Use() const
    {
        glUseProgram(id);
    }

    void SetVec3fv(const char* name, const glm::vec3& vals) const
    {
        glUniform3fv(glGetUniformLocation(id, name), 1, glm::value_ptr(vals));
    }

    void SetVec1f(const char* name, const float val) const
    {
        glUniform1f(glGetUniformLocation(id, name), val);
    }

    void SetVec1i(const char* name, const int val) const
    {
        glUniform1i(glGetUniformLocation(id, name), val);
    }

    void SetMat4fv(const char* name, const glm::mat4& vals) const
    {
        glUniformMatrix4fv(glGetUniformLocation(id, name),
            1, // number of matrices
            GL_FALSE, // should the matrices be transposed?
            glm::value_ptr(vals)); // pointer to data
    }

    // for uniform buffer objects
    void SetUniformBlock(const char* name, int val) const
    {
        unsigned int loc = glGetUniformBlockIndex(id, name);
        glUniformBlockBinding(id, loc, val);
    }

private:
    Shader vertexShader;
    Shader fragmentShader;
    Shader geometryShader;
    bool   hasGeometryShader;
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

