#include "Shader.h"

/*
==================
Helper function to read text file
==================
*/
static GLchar * ReadShader( const std::string &fileName );

/*
===================
Constructor/Deconstructor
===================
*/
Shader::Shader() : 
    m_program(0) 
{
}

Shader::~Shader() 
{
}

/*
====================
Load Shaders

    Compiles shader program from the sources contained in the files 
    vertFile and fragFile. Returns true on success, false on failure
====================
*/
bool Shader::Load(const std::string &vertFile, const std::string &fragFile)
{
    m_wasCompiled = false;
    m_program     = glCreateProgram();
    
    GLint compiledStatus[2]; // 0 = vert status, 1 = frag status

    /*
     * Init Vertex and fragment shaders
     */
    GLuint vertexShader   = glCreateShader( GL_VERTEX_SHADER   );
    GLuint fragmentShader = glCreateShader( GL_FRAGMENT_SHADER );

    /*
     * Read shader sources
     */
    const GLchar *vertSource = ReadShader( vertFile );
    const GLchar *fragSource = ReadShader( fragFile );
    if ( vertSource == NULL || fragSource == NULL ) {
        goto done;
    }

    /*
     * Compile vertex and fragment shader
     */
    glShaderSource( vertexShader,   1, &vertSource, NULL );
    glShaderSource( fragmentShader, 1, &fragSource, NULL );
    delete[] vertSource; // ReadShader() requires us to free buffer mem
    delete[] fragSource;

    glCompileShader( vertexShader   );
    glCompileShader( fragmentShader );

    glGetShaderiv( vertexShader,   GL_COMPILE_STATUS, &compiledStatus[0] );
    glGetShaderiv( fragmentShader, GL_COMPILE_STATUS, &compiledStatus[1] );

    for ( size_t i = 0; i < 2; ++i ) {
        GLuint shader = i ? fragmentShader : vertexShader;            
        
        if ( !compiledStatus[i] ) {
            GLsizei len;            
            glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &len );

            GLchar* log = new GLchar[len+1];
            glGetShaderInfoLog( shader, len, &len, log );
            //std::cerr << "Shader compilation failed: " << log << std::endl;
            printf("Shader comp failed: %s\n", log);
            // TODO - log
            delete [] log;

            goto done;
        }

        glAttachShader( m_program, shader );
    }

    /*
     * Link main shader program
     */
    glLinkProgram( m_program );

    GLint linkedStatus;
    glGetProgramiv( m_program, GL_LINK_STATUS, &linkedStatus );
    
    if ( !linkedStatus ) {

        GLsizei len;
        glGetProgramiv( m_program, GL_INFO_LOG_LENGTH, &len );

        GLchar* log = new GLchar[len+1];
        glGetProgramInfoLog( m_program, len, &len, log );
        //std::cerr << "Shader linking failed: " << log << std::endl;
        // TODO - logging
        printf("Shader linking failed: %s\n", log);
        delete [] log;
    }

    /*
     * Get and store uniform locations
     */
    m_viewMatLoc = glGetUniformLocation( m_program, "uViewMat" );
    m_projMatLoc = glGetUniformLocation( m_program, "uProjMat" );

    /* If we've reached this point then all is well */
    m_wasCompiled = true;

done:
#if 0
    if ( compiledStatus[0] ) {
        glDeleteShader( vertexShader );
    }
    if ( compiledStatus[1] ) {
        glDeleteShader( fragmentShader );
    }
#endif
    return m_wasCompiled;
}


/*
====================
GetProgram
====================
*/
GLuint Shader::GetProgram(void) const
{
    return m_program;
}

/*
====================
UseProgram
====================
*/
void Shader::Use(void) const
{
    glUseProgram( m_program );
}

/*
====================
Set World Matrices
====================
*/
void Shader::SetViewMat(const GLfloat *viewMat) const
{
    glUseProgram(m_program);

    glUniformMatrix4fv(m_viewMatLoc, 1, GL_FALSE, viewMat);
}

void Shader::SetProjMat(const GLfloat *projMat) const
{
    glUseProgram( m_program );
    
    glUniformMatrix4fv( m_projMatLoc, 1, GL_FALSE, projMat );
}

/*
====================
ReadShader

Stores the text from the shader file in a new GLchar *buffer;
The buffer needs to be delete[]ed after use
====================
*/
static GLchar * ReadShader( const std::string &fileName )
{
    FILE *infile = fopen( fileName.c_str(), "rb" );

    if ( !infile ) {
        // TODO - logging        
        return NULL;
    }

    fseek( infile, 0, SEEK_END );
    int len = ftell( infile );
    fseek( infile, 0, SEEK_SET );

    GLchar* source = new GLchar[len+1];

    fread( source, 1, len, infile );
    fclose( infile );

    source[len] = 0;

    return source;
}

