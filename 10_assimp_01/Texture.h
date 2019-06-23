#ifndef _TEXTURE_H_INCLUDED_
#define _TEXTURE_H_INCLUDED_

#include <string>

#include <GL/glew.h>

#include "../stb_image.h"

/*
===============================================================================
Texture

    Holds an OpenGL shader id, type (diffuse or spec), and image file location
===============================================================================
*/
class Texture
{
public:
    
    /*
    ===================
    Constructor/Deconstructor
    ===================
    */
    Texture(void);
    ~Texture(void);

    /*
    ====================
    Load Texture

        Creates the OpenGL texture from the image given by fileName in the
        directory given by dir. Returns true on success, false on failure
    ====================
    */
    bool Load(const std::string &fileName, const std::string &dir);

    /*
    ====================
    Bind
    
        Make the texture the current GL_TEXTURE_2D
    ====================
    */
    void Bind(void) const;

    /*
    ====================
    Getters
    ====================
    */
    GLuint      GetID(void)   const;
    std::string GetType(void) const;
    std::string GetPath(void) const;

    /*
    ====================
    Setters
    ====================
    */
    void SetType(const std::string &type);

private:
    GLuint      m_id;
    std::string m_type;
    std::string m_path;
};

#endif // _TEXTURE_H_INCLUDED_

