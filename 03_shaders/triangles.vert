
#version 400 core

layout( location = 0 ) in vec4 vPosition;

uniform vec4 uColor;

out vec4 color;

void
main()
{
    gl_Position = vPosition;
    color = uColor;
}
