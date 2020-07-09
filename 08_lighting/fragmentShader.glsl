#version 330 core
out vec4 FragColor;

uniform vec3 uObjectColor;
uniform vec3 uLightColor;

void main()
{
    //FragColor = vec4(0.5F, 1.0F, 0.2F, 1.0F);
    //FragColor = vertexColor;
    //FragColor = texture(texSampler, texCoord);
    //FragColor = texture(texSampler1, texCoord) * vertexColor;

    // 0.2 = 20% of second color, 80% of first color
    //FragColor = mix(texture(texSampler1, texCoord),
    //                texture(texSampler2, texCoord), 0.2) * vertexColor;
    //FragColor = mix(texture(texSampler1, texCoord),
    //                texture(texSampler2, texCoord), 0.2);

    FragColor = vec4(uLightColor * uObjectColor, 1.0F);
}

