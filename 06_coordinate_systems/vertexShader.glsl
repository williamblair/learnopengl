#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

// input from the CPU/main application
// global for all vertices (it is 'uniformly' the same for all vec inputs)
// uniform vec4 uVertexColor;

// input transformation matrix from CPU/main application
uniform mat4 uTransform;

out vec4 vertexColor;
out vec2 texCoord;

void main()
{
    //gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
    gl_Position = uTransform * vec4(aPos.x, aPos.y, aPos.z, 1.0);

    //vertexColor = uVertexColor; // updated by the main application each frame
    vertexColor = vec4(aColor, 1.0);
    texCoord = aTexCoord;
}
