#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 uModel;

void main()
{
    // just transform to world space; the geometry shader transforms
    // these to the respective light spaces for each cube map side
    gl_Position = uModel * vec4(aPos, 1.0);
}

