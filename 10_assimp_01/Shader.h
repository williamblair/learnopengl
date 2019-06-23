#ifndef _SHADER_H_INCLUDED_
#define _SHADER_H_INCLUDED_

#include <string>
#include <cstdio>

#include <GL/glew.h>

/*
===============================================================================
Shader

    Wraps an OpenGL shader program composed of a vertex and fragment shader
===============================================================================
*/
class Shader
{
public:
    
    /*
    ===================
    Constructor/Deconstructor
    ===================
    */
    Shader();
    ~Shader();

    /*
    ====================
    Load Shaders

        Compiles shader program from the sources contained in the files 
        vertFile and fragFile. Returns true on success, false on failure
    ====================
    */
    bool Load(const std::string &vertFile, const std::string &fragFile);

    /*
    ====================
    GetProgram
    ====================
    */
    GLuint GetProgram(void) const;

    /*
    ====================
    Use
    ====================
    */
    void Use(void) const;

private:
    /*
    ====================
    Member Variables
    ====================
    */
    GLuint m_program;
    bool   m_wasCompiled; // true if shader compilation was successful
};

#endif // _SHADER_H_INCLUDED_


