#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

// input from the CPU/main application
// global for all vertices (it is 'uniformly' the same for all vec inputs)
// uniform vec4 uVertexColor;

out vec4 vertexColor;

void main()
{
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
    //vertexColor = uVertexColor; // updated by the main application each frame
    vertexColor = vec4(aColor, 1.0);
}
