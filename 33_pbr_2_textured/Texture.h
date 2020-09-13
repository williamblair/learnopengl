#ifndef TEXTURE_H_INCLUDED
#define TEXTURE_H_INCLUDED

#include <iostream>
#include <string>
#include <cstdlib>

#define STB_IMAGE_IMPLEMENTATION
#include "../stb_image.h"

enum class TextureType {
    Diffuse,
    Specular
};

typedef struct Texture {
    std::string fileName;
    int width;
    int height;
    int numChannels;
    unsigned int id;
    TextureType type;
} Texture;

static void setTextureOptions()
{
    // assumes the texture is already currently bound to GL_TEXTURE_2D
    // s,t,r is the equivalent to x,y,z of texture axes
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

Texture createTexture(const std::string& fileName)
{
    Texture texture;

    texture.fileName = fileName;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(fileName.c_str(),
        &texture.width,
        &texture.height,
        &texture.numChannels,
        0);

    if (!data) {
        std::cout << "Failed to load texture data: " << fileName
            << std::endl;
        exit(EXIT_FAILURE);
    }

    GLenum format = GL_RED;
    if      (texture.numChannels == 1) format = GL_RED;
    else if (texture.numChannels == 3) format = GL_RGB;
    else if (texture.numChannels == 4) format = GL_RGBA;

    glGenTextures(1, &texture.id);
    glBindTexture(GL_TEXTURE_2D, texture.id);
    glTexImage2D(GL_TEXTURE_2D,
        0,  // mipmap level  = 0 = default; OpenGL auto chooses
        format, // how OpenGL will store the texture
        texture.width,
        texture.height,
        0,  // always 0
        format, // input format
        GL_UNSIGNED_BYTE,
        data);
    glGenerateMipmap(GL_TEXTURE_2D);

    setTextureOptions();

    stbi_image_free(data);

    return texture;
}

#endif // !TEXTURE_H_INCLUDED

