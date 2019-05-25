#version 450 core

in vec4 color;
in vec2 texCoord;

out vec4 fColor;

uniform sampler2D myTexture;
uniform sampler2D myTexture2;

uniform vec4 uLightColor;

void main()
{
    // just the texture
    //fColor = texture(myTexture, texCoord);

    // mix colors
    //fColor = texture(myTexture, texCoord) * color;

    // mix textures
    // 0.2 = percent of the second texture used (e.g. 80% first tex, 20% second tex)
    //fColor = mix(texture(myTexture, texCoord), texture(myTexture2, texCoord), 0.5);

    // color and lighting
    fColor = uLightColor * color;
}

