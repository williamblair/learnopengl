#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out VS_OUT
{
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} vs_out;

uniform mat4 uProjection;
uniform mat4 uView;
uniform mat4 uModel;

void main()
{
    gl_Position = uProjection * uView * uModel  * vec4(aPos, 1.0);

    mat3 normalMat = transpose(inverse(mat3(uModel)));
    vs_out.Normal = normalize(normalMat * aNormal);

    vs_out.TexCoords = aTexCoords;
}

