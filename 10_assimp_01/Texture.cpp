#include "Texture.h"


/*
===================
Constructor/Deconstructor
===================
*/
Texture::Texture(void) :
    m_id(0)
{
}

Texture::~Texture(void)
{
}

/*
====================
Load Texture

    Creates the OpenGL texture from the image given by fileName in the
    directory given by dir. Returns true on success, false on failure
====================
*/
bool Texture::Load(const std::string &fileName, const std::string &dir)
{
    bool wasLoaded = false; // true if the texture is loaded OK and returned
    printf("Filename: %s\n", fileName.c_str());

    std::string img_name(fileName);
#if 0
    std::size_t pos = img_name.find_last_of("\\");
    if (pos != std::string::npos) {
        img_name = img_name.substr(pos+1);
    } else {
        pos = img_name.find_last_of("/");
        if (pos != std::string::npos) {
            img_name = img_name.substr(pos+1);
        }
    }
#endif
    std::string fullpath = dir+"/"+img_name;
    
    /* 
     * Get the texture data from the image with stb_image 
     */
    int width, height, nChannels;
    unsigned char *data = stbi_load(fullpath.c_str(), &width, &height, &nChannels, 0);
    if (data == NULL) {
        // TODO - logging
        printf("Error loading texture: %s\n", fullpath.c_str());
        goto done;
    }

    /* 
     * Generate an opengl texture 
     */
    glGenTextures(1, &m_id);
    glBindTexture(GL_TEXTURE_2D, m_id);

    /* 
     * Set filtering parameters (optional?) 
     */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    /* 
     * Generate the texture image from texture data 
     */
    GLenum format;
    if      (nChannels == 1) format = GL_RED;
    else if (nChannels == 3) format = GL_RGB;
    else if (nChannels == 4) format = GL_RGBA;
    else {
        // TODO logging
        printf("LoadTexture: invalid number of channels: %d\n", nChannels);
        goto done;
    }
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    wasLoaded = true; // if we reached this point then all is well
    
done:
    if ( data != NULL ) {
        stbi_image_free(data);
    }
    return wasLoaded;
}

/*
====================
Bind
====================
*/
void Texture::Bind(void) const {
    glBindTexture( GL_TEXTURE_2D, m_id );
}

/*
====================
Getters
====================
*/
GLuint Texture::GetID(void) const {
    return m_id;
}
std::string Texture::GetType(void) const {
    return m_type;
}
std::string Texture::GetPath(void) const {
    return m_path;
}

/*
====================
Setters
====================
*/
void Texture::SetType(const std::string &type)
{
    m_type = type;
}

