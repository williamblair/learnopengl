#ifndef SSAO_KERNEL_H_INCLUDED
#define SSAO_KERNEL_H_INCLUDED

#include <vector>
#include <random>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

static inline float lerp(float a, float b, float f)
{
    return a + f * (b - a);
}

static std::uniform_real_distribution<GLfloat> randVals(0.0, 1.0);
static std::default_random_engine generator;

static std::vector<glm::vec3> createSSAOKernel()
{

    std::vector<glm::vec3> kernel;

    for (size_t i = 0; i <64; i++)
    {
        glm::vec3 sample(randVals(generator) * 2.0 - 1.0, 
                         randVals(generator) * 2.0 - 1.0, 
                         randVals(generator) * 2.0 - 1.0);
        sample = glm::normalize(sample);
        sample *= randVals(generator);
        float scale = float(i) / 64.0;

        // scale closer towards the center of the kernel
        scale = lerp(0.1f, 1.0f, scale * scale);
        sample *= scale;
        kernel.push_back(sample);
    }

    return kernel;
}

// 4x4 noise texture
typedef struct SSAONoiseTexture {
    GLuint id; // texture id
    std::vector<glm::vec3> textureVals;
} SSAONoiseTexture;
static SSAONoiseTexture createSSAONoise()
{
    SSAONoiseTexture tex;
    std::vector<glm::vec3> noiseTex;
    for (size_t i = 0; i < 16; i++)
    {
        glm::vec3 noise(randVals(generator) * 2.0 - 1.0,
                        randVals(generator) * 2.0 - 1.0,
                        0.0); // for rotating about the z axis in tangent space
        tex.textureVals.push_back(noise);
    }

    glGenTextures(1, &tex.id);
    glBindTexture(GL_TEXTURE_2D, tex.id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 4, 4, 0, GL_RGB, GL_FLOAT, &tex.textureVals[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    return tex;
}

#endif // !SSAO_KERNEL_H_INCLUDED

