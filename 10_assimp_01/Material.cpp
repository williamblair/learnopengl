#include "Material.h"

/*
===================
Constructor/Deconstructor
===================
*/
Material::Material() :
    m_shaderProg( 0 ),
    m_ambience(  1.0f, 0.5f, 0.31f ),
    m_diffuse (  1.0f, 0.5f, 0.31f ),
    m_specular(  0.5f, 0.5f, 0.5f  ),
    m_shininess( 32.0f )
{
}

Material::~Material()
{
}

/*
===================
Setters
===================
*/
void Material::SetShaderProg( const GLuint shaderProg ) {
    m_shaderProg = shaderProg;
}
void Material::SetAmbience( const glm::vec3 &ambience ) {
    m_ambience = glm::vec3( ambience );
}
void Material::SetDiffuse ( const glm::vec3 &diffuse  ) {
    m_diffuse = glm::vec3( diffuse ); 
}
void Material::SetSpecular( const glm::vec3 &specular ) {
    m_specular = glm::vec3( specular );
}
void Material::SetShininess( GLfloat shininess ) {
    m_shininess = shininess;
}


/*
===================
Send the phong properties to the shader uniforms
===================
*/
void Material::Update(void) const
{
    GLuint ambienceLoc = glGetUniformLocation( 
        m_shaderProg, 
        AMBIENT_UNIFORM_NAME
    );
    GLuint diffuseLoc = glGetUniformLocation( 
        m_shaderProg, 
        DIFFUSE_UNIFORM_NAME
    );
    GLuint specularLoc = glGetUniformLocation(
        m_shaderProg, 
        SPECULAR_UNIFORM_NAME
    );
    GLuint shininessLoc = glGetUniformLocation(
        m_shaderProg, 
        SHININESS_UNIFORM_NAME
    );

    glUniform3fv( ambienceLoc,  1, glm::value_ptr(m_ambience) );
    glUniform3fv( diffuseLoc,   1, glm::value_ptr(m_diffuse) );
    glUniform3fv( specularLoc,  1, glm::value_ptr(m_specular) );
    glUniform1f(  shininessLoc, m_shininess );
}


