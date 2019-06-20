#version 430 core
layout (location = 0) in vec4 vPosition;

uniform mat4 uModelMat;
uniform mat4 uViewMat;
uniform mat4 uProjMat;

void main()
{
    gl_Position = uProjMat * uViewMat * uModelMat * vec4(vPosition.x, vPosition.y, vPosition.z, 1.0);
}

