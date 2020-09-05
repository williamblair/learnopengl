#ifndef HDR_FRAMEBUFFER_H_INCLUDED
#define HDR_FRAMEBUFFER_H_INCLUDED

#include <glad/glad.h>

typedef struct HDRFrameBuffer {
    GLuint colorBufferIDs[2];
    GLuint renderBufferID;
    GLuint id;
    size_t width;
    size_t height;
} HDRFrameBuffer;

HDRFrameBuffer createHDRFrameBuffer() 
{
    HDRFrameBuffer frameBuffer;
    frameBuffer.width = 800; // screen width and height
    frameBuffer.height = 600;

    glGenFramebuffers(1, &frameBuffer.id);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer.id);

    glGenTextures(2, frameBuffer.colorBufferIDs);
    for (size_t i = 0; i < 2; i++)
    {
        glBindTexture(GL_TEXTURE_2D, frameBuffer.colorBufferIDs[i]);
        glTexImage2D(GL_TEXTURE_2D, 
                     0, 
                    GL_RGBA16F, // floating point in order to store values > 1.0 (GL_RGB clamps them)
                    frameBuffer.width,        // note if this size was different than window size we'd have to call glViewport() to render the full screen
                    frameBuffer.height, 
                    0, 
                    GL_RGBA, 
                    GL_FLOAT,  // GL_FLOAT now instead of GL_UNSIGNED_BYTE
                    NULL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // otherwise the blur filter would sample repeated texture vals
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
        glFramebufferTexture2D(GL_FRAMEBUFFER, 
                                GL_COLOR_ATTACHMENT0 + i,
                                GL_TEXTURE_2D, 
                                frameBuffer.colorBufferIDs[i], 
                                0);
    }

    glGenRenderbuffers(1, &frameBuffer.renderBufferID);
    glBindRenderbuffer(GL_RENDERBUFFER, frameBuffer.renderBufferID);
    glRenderbufferStorage(GL_RENDERBUFFER, 
                            GL_DEPTH_COMPONENT,
                            frameBuffer.width, 
                            frameBuffer.height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, 
                                GL_DEPTH_ATTACHMENT, 
                                GL_RENDERBUFFER, 
                                frameBuffer.renderBufferID);
    // tell OpenGL which color attachments we'll use for this framebuffer
    static GLuint attachments[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
    glDrawBuffers(2, attachments);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Framebuffer error: incomplete" << std::endl;
        exit(0);
    }
    //glBindRenderbuffer(GL_RENDERBUFFER, 0);


    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return frameBuffer;
}

#endif // !HDR_FRAMEBUFFER_H_INCLUDED

