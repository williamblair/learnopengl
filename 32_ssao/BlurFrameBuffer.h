#ifndef BLUR_FRAMEBUFFER_H_INCLUDED
#define BLUR_FRAMEBUFFER_H_INCLUDED

#include <glad/glad.h>

typedef struct BlurFrameBuffer {
    GLuint colorBufferIDs[2];
    GLuint renderBufferID;
    GLuint ids[2];
    size_t width;
    size_t height;
} BlurFrameBuffer;

BlurFrameBuffer createBlurFrameBuffer() 
{
    BlurFrameBuffer frameBuffer;
    frameBuffer.width = 800; // screen width and height
    frameBuffer.height = 600;

    glGenFramebuffers(2, frameBuffer.ids);
    glGenTextures(2, frameBuffer.colorBufferIDs);
    
    for (size_t i = 0; i < 2; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer.ids[i]);
        glBindTexture(GL_TEXTURE_2D, frameBuffer.colorBufferIDs[i]);

        glTexImage2D(GL_TEXTURE_2D, 
                    0, 
                    GL_RGBA16F, 
                    frameBuffer.width, 
                    frameBuffer.height, 
                    0, 
                    GL_RGBA, 
                    GL_FLOAT, 
                    NULL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // otherwise blur filter samples outside bounds
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frameBuffer.colorBufferIDs[i], 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cout << "Blur Framebuffer not complete" << std::endl;
        }
    }


    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return frameBuffer;
}

#endif // !BLUR_FRAMEBUFFER_H_INCLUDED

