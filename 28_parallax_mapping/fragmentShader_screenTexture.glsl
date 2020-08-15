#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D uScreenTexture;

void main()
{
    float depthVal = texture(uScreenTexture, TexCoords).r;
    FragColor = vec4(vec3(depthVal), 1.0);
    //FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}

