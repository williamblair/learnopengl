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
    glBindTexture(GL_TEXTURE_2D, depthMap.textureID);
    glTexImage2D(GL_TEXTURE_2D, 
                 0, 
                 GL_DEPTH_COMPONENT, 
                width, 
                height, 
                0, 
                GL_DEPTH_COMPONENT, 
                GL_FLOAT, 
                NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    // borders (anything outside of texture region) will be white
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    

    glBindFramebuffer(GL_FRAMEBUFFER, depthMap.framebufferID);
    glFramebufferTexture2D(GL_FRAMEBUFFER, 
                            GL_DEPTH_ATTACHMENT, 
                            GL_TEXTURE_2D, 
                            depthMap.textureID, 
                            0);
    glDrawBuffer(GL_NONE); // only the vertex shader is going to run; we are only concerned with depth,
    glReadBuffer(GL_NONE); // so no color buffer is necessary

    // unbind the frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return depthMap;
}

#endif // !DEPTH_MAP_H_INCLUDED

