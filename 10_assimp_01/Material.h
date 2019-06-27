#ifndef _MATERIAL_H_INCLUDED_
#define _MATERIAL_H_INCLUDED_

#include <GL/glew.h>

// matrix library
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// shader uniform names
#include "ShaderVariables.h"

/*
===============================================================================
Material

    Wraps phong material proprties and a texture, if needed
===============================================================================
*/
class Material
{
public:
    
    /*
    ===================
    Constructor/Deconstructor
    ===================
    */
    Material();
    ~Material();

    /*
    ===================
    Send the phong properties to the shader uniforms
    ===================
    */
    void Update(void) const;

    /*
    ===================
    Setters
        
        SetShaderProg needs to be called before the others so that we actually
        have a shader program id to send to
    ===================
    */
    void SetShaderProg( const GLuint shaderProg  );
    void SetAmbience(   const glm::vec3 &ambience );
    void SetDiffuse(    const glm::vec3 &diffuse  );
    void SetSpecular(   const glm::vec3 &specular ); 
    void SetShininess(  const GLfloat shininess   );
    
private:

    GLuint    m_shaderProg;
    
    glm::vec3 m_ambience;
    glm::vec3 m_diffuse;
    glm::vec3 m_specular;
    GLfloat   m_shininess; 
};

#endif


