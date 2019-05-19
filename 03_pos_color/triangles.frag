#version 450 core

out vec4 fColor;
in vec4 color;

void main()
{
    fColor = abs(color);
}
