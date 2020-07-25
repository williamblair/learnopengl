#version 330 core
layout (location = 0) in vec3 aPos;

// input transformation matrix from CPU/main application
uniform mat4 uTransform;

void main()
{
    gl_Position = uTransform * vec4(aPos.x, aPos.y, aPos.z, 1.0);
}
