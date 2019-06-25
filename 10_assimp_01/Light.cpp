#include "Light.h"


/*
===================
Constructor/Deconstructor
===================
*/
Light::Light() :
    m_ambient( 0.2f, 0.2f, 0.2f ),
    m_diffuse( 0.5f, 0.5f, 0.5f ),
    m_specular( 1.0f, 1.0f, 1.0f ) 
{
}

Light::~Light()
{
}

/*
===================
Create Shader Program

    Same as Load function of Shader class but also initializes the 
    internal cube.
===================
*/
bool Light::LoadShader(const std::string &vertFile, const std::string &fragFile)
{
    /*
     * Initialize the light's physical object
     */
    m_cube.init();
    m_cube.move( glm::vec3(0.0f, 0.0f, 0.0f) );
    m_cube.rotate( 0.0f, glm::vec3(0.0f, 1.0f, 0.0f) );
    m_cube.scale( glm::vec3(0.5f,0.5f,0.5f) );

    /*
     * Load the shader and tell the cube to use said shader
     */
    bool loadResult = m_shader.Load(vertFile, fragFile);
    if ( loadResult ) {
        m_cube.setShaderProg( m_shader.GetProgram() );
    }    

    return loadResult;
}


/*
===================
Set the main application shader id
===================
*/
void Light::SetWorldShaderProgram(GLuint id)
{
    m_worldShader = id;

    /*
     * Send our current light properties to the world shader
     */
    glUseProgram( m_worldShader );

    GLuint uniformPos  = glGetUniformLocation(m_worldShader, "uLightPos");
    GLuint ambientPos  = glGetUniformLocation(m_worldShader, "light.ambient");
    GLuint diffusePos  = glGetUniformLocation(m_worldShader, "light.diffuse");
    GLuint specularPos = glGetUniformLocation(m_worldShader, "light.specular");
    
    glUniform4fv( uniformPos, 1, glm::value_ptr(m_cube.getPos()) );

    glUniform3fv( ambientPos, 1, glm::value_ptr(m_ambient) );
    glUniform3fv( diffusePos, 1, glm::value_ptr(m_diffuse) );
    glUniform3fv( specularPos, 1, glm::value_ptr(m_specular) );

}


/*
===================
Set the location of the world shader's model matrix location, for our
internal cube to use when drawing.
===================
*/
void Light::SetModelMatLoc(GLuint id)
{
    m_cube.setModelMatLoc( id );
}

/*
===================
Get the shader program used by the light
===================
*/
GLuint Light::GetShaderProgram(void)
{
    return m_shader.GetProgram();
}

/*
===================
Draw
===================
*/
void Light::Draw(void)
{
    m_cube.draw();
}

