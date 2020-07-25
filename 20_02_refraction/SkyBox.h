#ifndef SKYBOX_H_INCLUDED
#define SKYBOX_H_INCLUDED

#include <vector>
#include <cstdlib>

#include <glad/glad.h>

#include "Texture.h"

typedef struct SkyBox {
    GLuint id;
    int width;
    int height;
    int numChannels;
} SkyBox;

SkyBox createSkyBox(const std::vector<std::string>& faces)
{
    SkyBox skyBox;

    glGenTextures(1, &skyBox.id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyBox.id);

    for (size_t i = 0; i < faces.size(); i++)
    {
        stbi_set_flip_vertically_on_load(false);
        unsigned char* data = stbi_load(faces[i].c_str(), 
                                        &skyBox.width, 
                                        &skyBox.height, 
                                        &skyBox.numChannels, 
                                        0);
        if (!data) {
            std::cout << "SkyBox error: failed to load " << faces[i] << std::endl;
            exit(0);
        }

        // POSITVE_X + i turns into negative x, positive, y, ...
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                    0, 
                    GL_RGB,
                    skyBox.width,
                    skyBox.height,
                    0,
                    skyBox.numChannels == 3 ? GL_RGB : GL_RGBA,
                    GL_UNSIGNED_BYTE,
                    data);

        stbi_image_free(data);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return skyBox;
}

#endif // !SKYBOX_H_INCLUDED

