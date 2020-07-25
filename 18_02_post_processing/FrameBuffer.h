#ifndef FRAMEBUFFER_H_INCLUDED
#define FRAMEBUFFER_H_INCLUDED

#include <glad/glad.h>

typedef struct FrameBuffer {
    GLuint texColorBufferID;
    GLuint renderBufferID;
    GLuint id;
    size_t width;
    size_t height;
} FrameBuffer;

FrameBuffer createFrameBuffer() 
{
    FrameBuffer frameBuffer;
    frameBuffer.width = 800;
    frameBuffer.height = 600;

    glGenFramebuffers(1, &frameBuffer.id);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer.id);

    glGenTextures(1, &frameBuffer.texColorBufferID);
    glBindTexture(GL_TEXTURE_2D, frameBuffer.texColorBufferID);
    glTexImage2D(GL_TEXTURE_2D, 
                 0, 
                GL_RGB, 
                frameBuffer.width,        // note if this size was different than window size we'd have to call glViewport() to render the full screen
                frameBuffer.height, 
                0, 
                GL_RGB, 
                GL_UNSIGNED_BYTE, 
                NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //glBindTexture(GL_TEXTURE_2D, 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, 
                            GL_COLOR_ATTACHMENT0, 
                            GL_TEXTURE_2D, 
                            frameBuffer.texColorBufferID, 
                            0);

    glGenRenderbuffers(1, &frameBuffer.renderBufferID);
    glBindRenderbuffer(GL_RENDERBUFFER, frameBuffer.renderBufferID);
    glRenderbufferStorage(GL_RENDERBUFFER, 
                            GL_DEPTH24_STENCIL8, // this acts as both depth and stencil buffer
                            frameBuffer.width, 
                            frameBuffer.height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, 
                                GL_DEPTH_STENCIL_ATTACHMENT, 
                                GL_RENDERBUFFER, 
                                frameBuffer.renderBufferID);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Framebuffer error: incomplete" << std::endl;
        exit(0);
    }
    //glBindRenderbuffer(GL_RENDERBUFFER, 0);


    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return frameBuffer;
}

#endif // !FRAMEBUFFER_H_INCLUDED

