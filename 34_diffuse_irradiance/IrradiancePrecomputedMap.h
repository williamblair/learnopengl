#ifndef IRRADIANCE_PRECOMPUTE_MAP_H_INCLUDED
#define IRRADIANCE_PRECOMPUTE_MAP_H_INCLUDED

#include <glad/glad.h>

typedef struct IrradiancePrecomputedMap
{
    //GLuint FBO; // frame buffer object
    //GLuint RBO; // render buffer object
    GLuint textureID;
} IrradiancePrecomputedMap;

IrradiancePrecomputedMap createIrradiancePrecomputedMap()
{
    IrradiancePrecomputedMap cubeMap;
    
    glGenTextures(1, &cubeMap.textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap.textureID);
    for (size_t i = 0; i < 6; i++)
    {
        // each face stored with 16bit floating point val
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                     0,
                     GL_RGB16F,
                     32, 32,
                     0,
                     GL_RGB,
                     GL_FLOAT,
                     nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //glGenFramebuffers(1, &cubeMap.FBO);
    //glGenRenderbuffers(1, &cubeMap.RBO);

    //glBindFramebuffer(GL_FRAMEBUFFER, cubeMap.FBO);
    //glBindRenderbuffer(GL_RENDERBUFFER, cubeMap.RBO);
    //glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    //glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, cubeMap.RBO);


    //glBindRenderbuffer(GL_RENDERBUFFER, 0);
    //glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return cubeMap;
}

#endif // !IRRADIANCE_PRECOMPUTE_MAP_H_INCLUDED

