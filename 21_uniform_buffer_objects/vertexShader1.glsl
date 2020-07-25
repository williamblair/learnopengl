#version 330 core
layout (location = 0) in vec3 aPos;

// uniform buffer object
// must be the same across all shader programs that use it
layout (std140) uniform uMatrices
{
    mat4 uProjection;
    mat4 uView;
};
uniform mat4 uModel;

void main()
{
    gl_Position = uProjection * uView * uModel * vec4(aPos.x, aPos.y, aPos.z, 1.0);
    //gl_Position = uView * uModel * vec4(aPos.x, aPos.y, aPos.z, 1.0);
}

