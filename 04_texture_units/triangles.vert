
#version 400 core

layout( location = 0 ) in vec4 vPosition;
layout( location = 1 ) in vec4 vColor;
layout( location = 2 ) in vec2 vTexCoord;

out vec4 color;
out vec2 texCoord;

void
main()
{
    gl_Position = vPosition;
    color = vColor;
    texCoord = vTexCoord;
}
