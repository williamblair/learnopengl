#ifndef SSAO_BLUR_BUFFER_H_INCLUDED
#define SSAO_BLUR_BUFFER_H_INCLUDED

#include <glad/glad.h>

typedef struct SSAOBlurBuffer {
    GLuint colorBufferID;
    GLuint renderBufferID;
    GLuint id;
    size_t width;
    size_t height;
} SSAOBlurBuffer;

SSAOBlurBuffer createSSAOBlurBuffer() 
{
    SSAOBlurBuffer frameBuffer;
    frameBuffer.width = 800; // screen width and height
    frameBuffer.height = 600;

    glGenFramebuffers(1, &frameBuffer.id);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer.id);

    glGenTextures(1, &frameBuffer.colorBufferID);

    // SSAO color output buffer
    glBindTexture(GL_TEXTURE_2D, frameBuffer.colorBufferID);
    glTexImage2D(GL_TEXTURE_2D, 
                 0, 
                GL_RED, // The output only has one value - the blurred occlusion value
                frameBuffer.width,        // note if this size was different than window size we'd have to call glViewport() to render the full screen
                frameBuffer.height, 
                0, 
                GL_RED, 
                GL_FLOAT,  // GL_FLOAT now instead of GL_UNSIGNED_BYTE
                NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, 
                            GL_COLOR_ATTACHMENT0,
                            GL_TEXTURE_2D, 
                            frameBuffer.colorBufferID, 
                            0);
    
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Framebuffer error: incomplete" << std::endl;
        exit(0);
    }
    //glBindRenderbuffer(GL_RENDERBUFFER, 0);


    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return frameBuffer;
}

#endif // !SSAO_BLUR_BUFFER_H_INCLUDED

