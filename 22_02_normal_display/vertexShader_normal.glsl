#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

// sends to geometry shader
out VS_OUT
{
    vec3 normal;
} vs_out;

uniform mat4 uView;
uniform mat4 uModel;

void main()
{
    gl_Position = uView * uModel * vec4(aPos, 1.0);
    mat3 normalMatrix = mat3(transpose(inverse(uView * uModel)));
    vs_out.normal = normalize(vec3(vec4(normalMatrix * aNormal, 0.0)));
}

