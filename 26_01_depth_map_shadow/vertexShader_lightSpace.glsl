#version 330 core
layout (location = 0) in vec3 aPos;

// input light space transformation matrix
uniform mat4 uTransform;

// model matrix for transforming the position to world space for lighting calc
uniform mat4 uModel;

void main()
{
    gl_Position = uTransform * uModel * vec4(aPos, 1.0);
}

