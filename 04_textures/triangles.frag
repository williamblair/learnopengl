#version 450 core

in vec4 color;
in vec2 texCoord;

out vec4 fColor;

uniform sampler2D myTexture;

void main()
{
    // just the texture
    fColor = texture(myTexture, texCoord);

    // mix colors
    //fColor = texture(myTexture, texCoord) * color;
}
