#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

// instead of sampler2D its a cube sampler
uniform samplerCube uSkyBoxSampler;

void main()
{
    FragColor = texture(uSkyBoxSampler, TexCoords);
}

