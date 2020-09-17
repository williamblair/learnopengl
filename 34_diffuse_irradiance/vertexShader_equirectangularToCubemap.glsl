#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 localPos;

uniform mat4 uProjection;
uniform mat4 uView;

void main()
{
    localPos = aPos;
    gl_Position = uProjection * uView * vec4(localPos, 1.0);
}

