
#version 400 core

layout( location = 0 ) in vec4 vPosition;
layout( location = 1 ) in vec4 vColor;
layout( location = 2 ) in vec2 vTexCoord;
layout( location = 3 ) in vec3 vNormal;

out vec4 color;
out vec2 texCoord;
out vec3 normal;
out vec3 fragPos;

uniform mat4 uModelMat;
uniform mat4 uViewMat;
uniform mat4 uProjMat;

void
main()
{
    gl_Position = uProjMat * uViewMat * uModelMat * vec4(vPosition.x, vPosition.y, vPosition.z, 1.0);
    fragPos = vec3(uModelMat * vPosition);
    color = vColor;
    texCoord = vTexCoord;
    // This is inefficient and should not be done here!
    //    should calculate the normal matrix on the CPU and send it
    //      to the shaders via a uniform before drawing (the matrix part i mean)
    normal = mat3(transpose(inverse(uModelMat))) * vNormal;
}
