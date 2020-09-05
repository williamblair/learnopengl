#ifndef DEPTH_MAP_H_INCLUDED
#define DEPTH_MAP_H_INCLUDED

#include <glad/glad.h>

typedef struct DepthMap
{
    GLuint textureID;
    GLuint framebufferID;
    GLuint width; // frame buffer size
    GLuint height;
} DepthMap;

DepthMap createDepthMap()
{
    DepthMap depthMap;

    const size_t width = 1024;
    const size_t height = 1024;

    depthMap.width = width;
    depthMap.height = height;

    glGenFramebuffers(1, &depthMap.framebufferID);

    glGenTextures(1, &depthMap.textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depthMap.textureID);

    for (size_t i = 0; i < 6; i++)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                     0,
                     GL_DEPTH_COMPONENT,
                     width,
                     height,
                     0,
                     GL_DEPTH_COMPONENT,
                     GL_FLOAT,
                     NULL);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMap.framebufferID);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthMap.textureID, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return depthMap;
}

#endif // !DEPTH_MAP_H_INCLUDED

