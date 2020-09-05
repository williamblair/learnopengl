#ifndef DEFERRED_FRAMEBUFFER_H_INCLUDED
#define DEFERRED_FRAMEBUFFER_H_INCLUDED

#include <glad/glad.h>

typedef struct DeferredRenderBuffer {
    GLuint colorBufferIDs[3];
    GLuint renderBufferID;
    GLuint id;
    size_t width;
    size_t height;
} DeferredRenderBuffer;

DeferredRenderBuffer createDeferredRenderBuffer() 
{
    DeferredRenderBuffer frameBuffer;
    frameBuffer.width = 800; // screen width and height
    frameBuffer.height = 600;

    glGenFramebuffers(1, &frameBuffer.id);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer.id);

    glGenTextures(3, frameBuffer.colorBufferIDs);

    // Position buffer
    glBindTexture(GL_TEXTURE_2D, frameBuffer.colorBufferIDs[0]);
    glTexImage2D(GL_TEXTURE_2D, 
                 0, 
                GL_RGBA16F, // floating point in order to store values > 1.0 (GL_RGB clamps them)
                frameBuffer.width,        // note if this size was different than window size we'd have to call glViewport() to render the full screen
                frameBuffer.height, 
                0, 
                GL_RGBA, 
                GL_FLOAT,  // GL_FLOAT now instead of GL_UNSIGNED_BYTE
                NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, 
                            GL_COLOR_ATTACHMENT0,
                            GL_TEXTURE_2D, 
                            frameBuffer.colorBufferIDs[0], 
                            0);

    // normal buffer
    glBindTexture(GL_TEXTURE_2D, frameBuffer.colorBufferIDs[1]);
    glTexImage2D(GL_TEXTURE_2D, 
                 0, 
                GL_RGBA16F, // floating point in order to store values > 1.0 (GL_RGB clamps them)
                frameBuffer.width,        // note if this size was different than window size we'd have to call glViewport() to render the full screen
                frameBuffer.height, 
                0, 
                GL_RGBA, 
                GL_FLOAT,  // GL_FLOAT now instead of GL_UNSIGNED_BYTE
                NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, 
                            GL_COLOR_ATTACHMENT1,
                            GL_TEXTURE_2D, 
                            frameBuffer.colorBufferIDs[1], 
                            0);

    // color (albedo) and specular buffer
    glBindTexture(GL_TEXTURE_2D, frameBuffer.colorBufferIDs[2]);
    glTexImage2D(GL_TEXTURE_2D, 
                 0, 
                GL_RGBA, 
                frameBuffer.width,        // note if this size was different than window size we'd have to call glViewport() to render the full screen
                frameBuffer.height, 
                0, 
                GL_RGBA, 
                GL_UNSIGNED_BYTE,
                NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, 
                            GL_COLOR_ATTACHMENT2,
                            GL_TEXTURE_2D, 
                            frameBuffer.colorBufferIDs[2], 
                            0);

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
    static GLuint attachments[3] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
    glDrawBuffers(3, attachments);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Framebuffer error: incomplete" << std::endl;
        exit(0);
    }
    //glBindRenderbuffer(GL_RENDERBUFFER, 0);


    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return frameBuffer;
}

#endif // !DEFERRED_FRAMEBUFFER_H_INCLUDED

