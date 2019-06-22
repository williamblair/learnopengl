
#version 400 core

layout( location = 0 ) in vec4 vPosition;
layout( location = 1 ) in vec4 vColor;

out vec4 color;

void
main()
{
    // make the triangle upside down
    gl_Position = vec4(vPosition.x, -vPosition.y, vPosition.z, vPosition.w);
    color = vColor;
}
